#include "Engine_pch.h"

#include "Platform/DirectX12/Public/D3D12CommandContext.h"

#include "Platform/Public/Utility/APIBridge/D3DUtility.h"
#include "Platform/DirectX12/Private/D3D12CommandManager.h"
#include "Platform/DirectX12/Public/Resource/D3D12ColorBuffer.h"
#include "Platform/DirectX12/Public/Resource/D3D12IndexBuffer.h"
#include "Platform/DirectX12/Public/Resource/D3D12VertexBuffer.h"
#include "Platform/DirectX12/Public/Resource/D3D12DepthBuffer.h"
#include "Platform/DirectX12/Public/D3D12PipelineState.h"
#include "Platform/DirectX12/Public/D3D12RootSignature.h"
#include "Platform/DirectX12/Public/D3D12DescriptorHeap.h"
#include "Platform/DirectX12/Public/ResourceManagement/D3D12ConstantBufferManager.h"
#include "Platform/DirectX12/Public/Resource/D3D12Texture.h"
#include "Runtime/Graphics/Public/ResourceManagement/ITextureManager.h"

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
				, m_DynamicViewDescriptorHeap(*this, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
				, m_DynamicSamplerDescriptorHeap(*this, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
				, m_CpuLinearAllocator(LAT_CpuWritable)
				, m_GpuLinearAllocator(LAT_GpuExclusive)
			{
				ZeroMem(m_ResourceBarrierBuffer, _countof(m_ResourceBarrierBuffer) * sizeof(D3D12_RESOURCE_BARRIER));
				m_D3DCmdListType = PlatformUtils::IECommandListTypeToD3DCommandListType(Type);
				ZeroMem(m_CurrentDescriptorHeaps, sizeof(m_CurrentDescriptorHeaps));
			}

			D3D12CommandContext::~D3D12CommandContext()
			{
				UnInitialize();
			}

			void D3D12CommandContext::BindDescriptorHeaps()
			{
				UINT NonNullHeaps = 0;
				ID3D12DescriptorHeap* HeapsToBind[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
				for (UINT i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
				{
					ID3D12DescriptorHeap* HeapIter = m_CurrentDescriptorHeaps[i];
					if (HeapIter != nullptr)
						HeapsToBind[NonNullHeaps++] = HeapIter;
				}

				if (NonNullHeaps > 0)
					m_pID3D12CommandList->SetDescriptorHeaps(NonNullHeaps, HeapsToBind);
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
				BindDescriptorHeaps();

			}
			
			void D3D12CommandContext::OMSetRenderTargets(UInt32 NumRTVs, const IColorBuffer* Targets[], IDepthBuffer* pDepthBuffer)
			{
				constexpr UInt32 cx_MaxRTVBinds = 12;
				D3D12_CPU_DESCRIPTOR_HANDLE RTVHandles[cx_MaxRTVBinds];
				ZeroMem(&RTVHandles, sizeof(D3D12_CPU_DESCRIPTOR_HANDLE) * cx_MaxRTVBinds);

				for (UInt32 i = 0; i < NumRTVs; ++i)
				{
					const D3D12ColorBuffer* pBuffer = DCast<const D3D12ColorBuffer*>(Targets[i]);
					IE_ASSERT(pBuffer != NULL);

					RTVHandles[i] = pBuffer->GetRTVHandle();
				}

				D3D12_CPU_DESCRIPTOR_HANDLE DSVHandle;
				DSVHandle.ptr = NULL;
				if (pDepthBuffer != NULL)
				{
					D3D12DepthBuffer* pD3D12DepthBuffer = DCast<D3D12DepthBuffer*>(pDepthBuffer);
					DSVHandle = pD3D12DepthBuffer->GetDSV();
				}
				
				m_pID3D12CommandList->OMSetRenderTargets(NumRTVs, RTVHandles, false, &DSVHandle);
			}

			void D3D12CommandContext::ClearDepth(IDepthBuffer& DepthBuffer)
			{
				D3D12DepthBuffer* pD3D12DepthBuffer = DCast<D3D12DepthBuffer*>(&DepthBuffer);
				D3D12_CPU_DESCRIPTOR_HANDLE  DSVHandle = pD3D12DepthBuffer->GetDSV();

				m_pID3D12CommandList->ClearDepthStencilView(DSVHandle, D3D12_CLEAR_FLAG_DEPTH, DepthBuffer.GetClearDepth(), DepthBuffer.GetClearStencil(), 0, NULL);
			}

			void D3D12CommandContext::RSSetViewPorts(UInt32 NumViewPorts, const ViewPort* ViewPorts)
			{
				m_pID3D12CommandList->RSSetViewports(NumViewPorts, RCast<const D3D12_VIEWPORT*>(ViewPorts));
			}

			void D3D12CommandContext::RSSetScissorRects(UInt32 NumScissorRects, const Rect* ScissorRects)
			{
				m_pID3D12CommandList->RSSetScissorRects(NumScissorRects, RCast<const RECT*>(ScissorRects));
			}

			void D3D12CommandContext::SetPrimitiveTopologyType(EPrimitiveTopology TopologyType)
			{
				m_pID3D12CommandList->IASetPrimitiveTopology((D3D12_PRIMITIVE_TOPOLOGY)TopologyType);
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

			void D3D12CommandContext::SetDescriptorHeap(EResourceHeapType Type, IDescriptorHeap* HeapPtr)
			{
				D3D12DescriptorHeap* pID3D12Heap = RCast<D3D12DescriptorHeap*>(HeapPtr);
				IE_ASSERT(pID3D12Heap != NULL);

				if (m_CurrentDescriptorHeaps[Type] != pID3D12Heap->GetHeapPointer())
				{
					m_CurrentDescriptorHeaps[Type] = pID3D12Heap->GetHeapPointer();
					BindDescriptorHeaps();
				}
			}

			DynAlloc D3D12CommandContext::ReserveUploadMemory(UInt64 SizeInBytes)
			{
				return m_CpuLinearAllocator.Allocate(SizeInBytes);
			}

			void D3D12CommandContext::UpdateSubresources(IGPUResource& Destination, IGPUResource& Intermediate, UInt32 IntermediateOffset, UInt32 FirstSubresource, UInt32 NumSubresources, SubResourceData& SubresourceData)
			{
				D3D12GPUResource* pD3D12DestGpuResource = DCast<D3D12GPUResource*>(&Destination);
				D3D12GPUResource* pD3D12IntGpuResource = DCast<D3D12GPUResource*>(&Intermediate);

				ID3D12Resource* pID3D12Destination = pD3D12DestGpuResource->GetResource();
				ID3D12Resource* pID3D12Intermediate = pD3D12IntGpuResource->GetResource();
				D3D12_SUBRESOURCE_DATA SRData = {};
				SRData.pData = SubresourceData.pData;
				SRData.RowPitch = SubresourceData.RowPitch;
				SRData.SlicePitch = SubresourceData.SlicePitch;

				::UpdateSubresources(m_pID3D12CommandList, pID3D12Destination, pID3D12Intermediate, IntermediateOffset, FirstSubresource, NumSubresources, &SRData);
			}

			void D3D12CommandContext::BindVertexBuffer(UInt32 Slot, IVertexBuffer& VertexBuffer)
			{
				D3D12_VERTEX_BUFFER_VIEW* pView = RCast<D3D12_VERTEX_BUFFER_VIEW*>(VertexBuffer.GetNativeBufferView());
				m_pID3D12CommandList->IASetVertexBuffers(Slot, 1, pView);
			}

			void D3D12CommandContext::BindIndexBuffer(IIndexBuffer& IndexBuffer)
			{
				D3D12_INDEX_BUFFER_VIEW* pView = RCast<D3D12_INDEX_BUFFER_VIEW*>(IndexBuffer.GetNativeBufferView());
				m_pID3D12CommandList->IASetIndexBuffer(pView);
			}

			void D3D12CommandContext::SetGraphicsConstantBuffer(UInt32 Index, IConstantBuffer* pConstantBuffer)
			{
				D3D12ConstantBuffer& D3D12Cb = *DCast<D3D12ConstantBuffer*>(pConstantBuffer);
				D3D12Cb.UploadBuffer();
				D3D12_GPU_VIRTUAL_ADDRESS Address = D3D12Cb.GetGPUVirtualAddress();
				m_pID3D12CommandList->SetGraphicsRootConstantBufferView(Index, Address);
			}

			void D3D12CommandContext::SetTexture(UInt32 Slot, ITextureRef& pTexture)
			{
				const D3D12Texture* pD3D12Tex = DCast<const D3D12Texture*>(pTexture.Get());
				IE_ASSERT(pD3D12Tex != NULL);

				D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle{ pD3D12Tex->GetShaderVisibleDescriptorHandle().GetGpuPtr() };
				m_pID3D12CommandList->SetGraphicsRootDescriptorTable(Slot, GpuHandle);
			}

			void D3D12CommandContext::SetPipelineState(IPipelineState& Pipeline)
			{
				ID3D12PipelineState* pD3D12Pipeline = RCast<ID3D12PipelineState*>(Pipeline.GetNativePSO());
				IE_ASSERT(pD3D12Pipeline != NULL);

				m_pID3D12CommandList->SetPipelineState(pD3D12Pipeline);
			}

			void D3D12CommandContext::SetGraphicsRootSignature(IRootSignature& Signature)
			{
				ID3D12RootSignature* pD3D12Signature = RCast<ID3D12RootSignature*>(Signature.GetNativeSignature());
				IE_ASSERT(pD3D12Signature != NULL);

				m_pID3D12CommandList->SetGraphicsRootSignature(pD3D12Signature);
			}

			void D3D12CommandContext::Draw(UInt32 VertexCount, UInt32 VertexStartOffset) 
			{
				DrawInstanced(VertexCount, 1, VertexStartOffset, 0);
			}
			
			void D3D12CommandContext::DrawIndexed(UInt32 IndexCount, UInt32 StartIndexLocation, Int32 BaseVertexLocation) 
			{
				DrawIndexedInstanced(IndexCount, 1, StartIndexLocation, BaseVertexLocation, 0);
			}
			
			void D3D12CommandContext::DrawInstanced(UInt32 VertexCountPerInstance, UInt32 InstanceCount, UInt32 StartVertexLocation, UInt32 StartInstanceLocation)
			{
				FlushResourceBarriers();
				m_DynamicViewDescriptorHeap.CommitGraphicsRootDescriptorTables(m_pID3D12CommandList);
				m_DynamicSamplerDescriptorHeap.CommitGraphicsRootDescriptorTables(m_pID3D12CommandList);
				m_pID3D12CommandList->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
			}
			
			void D3D12CommandContext::DrawIndexedInstanced(UInt32 IndexCountPerInstance, UInt32 InstanceCount, UINT StartIndexLocation, UInt32 BaseVertexLocation, UInt32 StartInstanceLocation) 
			{
				FlushResourceBarriers();
				m_DynamicViewDescriptorHeap.CommitGraphicsRootDescriptorTables(m_pID3D12CommandList);
				m_DynamicSamplerDescriptorHeap.CommitGraphicsRootDescriptorTables(m_pID3D12CommandList);
				m_pID3D12CommandList->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
			}

			void D3D12CommandContext::TransitionResource(IGPUResource& Resource, EResourceState NewState, bool FlushImmediate/* = false*/)
			{
				EResourceState OldState = Resource.GetUsageState();

				if (NewState != OldState)
				{
					IE_ASSERT(m_NumBarriersToFlush < 16, "Exceeded arbitrary limit on buffered barriers.");
					D3D12_RESOURCE_BARRIER& BarrierDesc = m_ResourceBarrierBuffer[m_NumBarriersToFlush++];
					

					D3D12GPUResource* res = DCast<D3D12GPUResource*>(&Resource);


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
				
				D3D12CommandQueue* pQueue = DCast<D3D12CommandQueue*>(g_pCommandManager->GetGraphicsQueue());
				UInt64 FenceValue = pQueue->ExecuteCommandList(m_pID3D12CommandList);

				if (WaitForCompletion)
					g_pCommandManager->WaitForFence(FenceValue);

				m_pID3D12CommandList->Reset(m_pID3D12CurrentCmdAllocator, NULL);

				// TODO: Set root signature

				BindDescriptorHeaps();

				return 0;

			}

			UInt64 D3D12CommandContext::Finish(bool WaitForCompletion/* = false*/)
			{
				FlushResourceBarriers();

				
				D3D12CommandQueue* pQueue = DCast<D3D12CommandQueue*>(g_pCommandManager->GetQueue(m_Type));

				UInt64 FenceValue = pQueue->ExecuteCommandList(m_pID3D12CommandList);
				pQueue->DiscardAllocator(FenceValue, m_pID3D12CurrentCmdAllocator);
				m_pID3D12CurrentCmdAllocator = NULL;

				m_CpuLinearAllocator.CleanupUsedPages(FenceValue);
				m_GpuLinearAllocator.CleanupUsedPages(FenceValue);
				m_DynamicViewDescriptorHeap.CleanupUsedHeaps(FenceValue);
				m_DynamicSamplerDescriptorHeap.CleanupUsedHeaps(FenceValue);

				if (WaitForCompletion)
					pQueue->WaitForFence(FenceValue);

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
