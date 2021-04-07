#include "Engine_pch.h"

#include "Platform/DirectX12/Public/D3D12CommandContext.h"

#include "Platform/DirectX12/Private/D3D12CommandManager.h"
#include "Platform/Public/Utility/APIBridge/D3DUtility.h"
#include "Platform/DirectX12/Public/Resource/D3D12ColorBuffer.h"

#include "Runtime/Graphics/Public/GraphicsCore.h"


namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{

			// ----------------
			//  Context Manager
			// ----------------

			D3D12ContextManager::D3D12ContextManager()
			{
			}

			D3D12ContextManager::~D3D12ContextManager()
			{
			}

			ICommandContext* D3D12ContextManager::AllocateContext(ECommandListType Type)
			{
				std::lock_guard<std::mutex> LockGuard(m_ContextAllocationMutex);

				auto& AvailableContexts = m_AvailableContexts[Type];

				ICommandContext* ret = nullptr;
				if (AvailableContexts.empty())
				{
					ret = new D3D12CommandContext(Type);
					m_ContextPool[Type].emplace_back(ret);
					ret->Initialize();
				}
				else
				{
					ret = AvailableContexts.front();
					AvailableContexts.pop();
					ret->Reset();
				}
				IE_ASSERT(ret != nullptr);

				D3D12CommandContext* pD3D12Context = DCast<D3D12CommandContext*>(ret);
				IE_ASSERT(pD3D12Context != NULL);

				IE_ASSERT(pD3D12Context->m_Type == Type);

				return ret;
			}

			void D3D12ContextManager::FreeContext(ICommandContext* pContext)
			{
				IE_ASSERT(pContext != NULL);
				std::lock_guard<std::mutex> LockGuard(m_ContextAllocationMutex);

				D3D12CommandContext* pD3D12Context = DCast<D3D12CommandContext*>(pContext);
				IE_ASSERT(pD3D12Context != NULL)

					m_AvailableContexts[pD3D12Context->m_Type].push(pContext);
			}

			void D3D12ContextManager::DestroyAllContexts()
			{

			}


			// ----------------
			//  Command Context
			// ----------------

			D3D12CommandContext::D3D12CommandContext(const ECommandListType& Type)
				: ICommandContext(Type)
				, m_NumBarriersToFlush(0u)
				, m_pID3D12CommandList(NULL)
				, m_pID3D12CurrentCmdAllocator(NULL)
			{
				ZeroMem(m_ResourceBarrierBuffer, _countof(m_ResourceBarrierBuffer) * sizeof(D3D12_RESOURCE_BARRIER));
				m_D3DCmdListType = PlatformUtils::IECommandListTypeToD3DCommandListType(Type);
			}

			D3D12CommandContext::~D3D12CommandContext()
			{
				UnInitialize();
			}

			void D3D12CommandContext::Initialize()
			{
				ICommandContext* pThisContext = RCast<ICommandContext*>(this);
				void** pThisAllocator = RCast<void**>(&m_pID3D12CurrentCmdAllocator);

				g_pCommandManager->CreateNewCommandContext(m_Type, &pThisContext, pThisAllocator);
			}

			void D3D12CommandContext::UnInitialize()
			{
				COM_SAFE_RELEASE(m_pID3D12CommandList);
			}

			void D3D12CommandContext::Reset()
			{
				// We only call Reset() on previously freed contexts.  The command list persists, but we must
				// request a new allocator.
				IE_ASSERT(m_pID3D12CommandList != nullptr && m_pID3D12CurrentCmdAllocator == nullptr);

				m_pID3D12CurrentCmdAllocator = RCast<D3D12CommandQueue*>(g_pCommandManager->GetQueue(m_Type))->RequestAllocator();
				m_pID3D12CommandList->Reset(m_pID3D12CurrentCmdAllocator, NULL);

				// TODO Reset root signature
			}

			void D3D12CommandContext::RSSetViewPorts(UInt32 NumViewPorts, const ViewPort* ViewPorts)
			{
				m_pID3D12CommandList->RSSetViewports(NumViewPorts, RCast<const D3D12_VIEWPORT*>(ViewPorts));
			}

			void D3D12CommandContext::RSSetScissorRects(UInt32 NumScissorRects, const Rect* ScissorRects)
			{
				m_pID3D12CommandList->RSSetScissorRects(NumScissorRects, RCast<const RECT*>(ScissorRects));
			}

			void D3D12CommandContext::ClearColorBuffer(IColorBuffer& Buffer, Rect& Rect)
			{
				FlushResourceBarriers();
				D3D12ColorBuffer* pD3D12ColorBuffer = DCast<D3D12ColorBuffer*>(&Buffer);
				IE_ASSERT(pD3D12ColorBuffer != NULL);

				FVector4 ClearColor = pD3D12ColorBuffer->GetClearColor().ToVector4();
				m_pID3D12CommandList->ClearRenderTargetView(pD3D12ColorBuffer->GetRTVHandle(), &ClearColor.x, 1, RCast<const D3D12_RECT*>(&Rect));
			}

			void D3D12CommandContext::CreateTexture2D()
			{
			}

			void D3D12CommandContext::CreateBuffer()
			{
			}

			void D3D12CommandContext::BindVertexBuffer(const ieVertexBuffer& Vertexbuffer)
			{
			}

			void D3D12CommandContext::BindIndexBuffer(const ieIndexBuffer& IndexBuffer)
			{
			}

			void D3D12CommandContext::DrawMesh()
			{
			}

			void D3D12CommandContext::TransitionResource(IGPUResource& Resource, EResourceState NewState, bool FlushImmediate/* = false*/)
			{
				EResourceState OldState = Resource.GetUsageState();

				if (NewState != OldState)
				{
					IE_ASSERT(m_NumBarriersToFlush < 16, "Exceeded arbitrary limit on buffered barriers");
					D3D12_RESOURCE_BARRIER& BarrierDesc = m_ResourceBarrierBuffer[m_NumBarriersToFlush++];

					BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					BarrierDesc.Transition.pResource = DCast<D3D12GPUResource*>(&Resource)->GetResource();
					BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
					BarrierDesc.Transition.StateBefore = (D3D12_RESOURCE_STATES)OldState;
					BarrierDesc.Transition.StateAfter = (D3D12_RESOURCE_STATES)NewState;

					// Check to see if we already started the transition
					if (NewState == Resource.GetTransitionState())
					{
						BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_END_ONLY;
						Resource.SetTransitionState((EResourceState)-1);
					}
					else
						BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

					Resource.SetUsageState(NewState);
				}
				//else if (NewState == D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
				//	InsertUAVBarrier(Resource, FlushImmediate);

				if (FlushImmediate || m_NumBarriersToFlush == 16)
					FlushResourceBarriers();
			}


			UInt64 D3D12CommandContext::Flush(bool WaitForCompletion/* = false*/)
			{
				FlushResourceBarriers();

				IE_ASSERT(m_pID3D12CurrentCmdAllocator != NULL);

				UInt64 FenceValue = DCast<D3D12CommandQueue*>(g_pCommandManager->GetGraphicsQueue())->ExecuteCommandList(m_pID3D12CommandList);

				if (WaitForCompletion)
					g_pCommandManager->WaitForFence(FenceValue);

				m_pID3D12CommandList->Reset(m_pID3D12CurrentCmdAllocator, NULL);

				// TODO: Set root signature
				// TODO: Bind descriptor heaps

				return 0;

			}

			UInt64 D3D12CommandContext::Finish(bool WaitForCompletion/* = false*/)
			{
				FlushResourceBarriers();

				D3D12CommandQueue* pQueue = DCast<D3D12CommandQueue*>(g_pCommandManager->GetQueue(m_Type));

				UInt64 FenceValue = pQueue->ExecuteCommandList(m_pID3D12CommandList);
				pQueue->DiscardAllocator(FenceValue, m_pID3D12CurrentCmdAllocator);
				m_pID3D12CurrentCmdAllocator = NULL;

				// TODO Cleanup dynamic descriptor heaps

				if (WaitForCompletion)
					pQueue->WaitforFence(FenceValue);

				g_pContextManager->FreeContext(this);

				return 0;
			}

			void D3D12CommandContext::FlushResourceBarriers()
			{
				if (m_NumBarriersToFlush > 0)
				{
					m_pID3D12CommandList->ResourceBarrier(m_NumBarriersToFlush, m_ResourceBarrierBuffer);
					m_NumBarriersToFlush = 0u;
				}
			}

		}
	}
}
