#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/ICommandContext.h"
#include "Runtime/Graphics/Private/ICommandManager.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{

			class INSIGHT_API D3D12ContextManager final : public IContextManager
			{
				friend class IRenderContextFactory;
				friend class D3D12RenderContextFactory;
			public:
				virtual ICommandContext* AllocateContext(ECommandListType Type) override;
				virtual void FreeContext(ICommandContext* pContext) override;
				virtual void DestroyAllContexts() override;

			protected:
				D3D12ContextManager();
				virtual ~D3D12ContextManager();
			};



			class INSIGHT_API D3D12CommandContext : public ICommandContext
			{
				friend D3D12ContextManager;
			public:
				virtual void* GetNativeContext() override { return RCast<void*>(m_pID3D12CommandList); }
				virtual void** GetNativeContextAddress() override { return RCast<void**>(&m_pID3D12CommandList);  }
				virtual void  Initialize() override;
				virtual void  UnInitialize() override;
				virtual void  Reset() override;

				virtual void RSSetViewPorts(UInt32 NumViewPorts, const ViewPort* ViewPorts) override;
				virtual void RSSetScissorRects(UInt32 NumScissorRects, const Rect* ScissorRects) override;
				
				virtual void ClearColorBuffer(IColorBuffer& Buffer, Rect& Rect) override;

				virtual void CreateTexture2D() override;
				virtual void CreateBuffer() override;

				virtual void BindVertexBuffer(const ieVertexBuffer& Vertexbuffer) override;
				virtual void BindIndexBuffer(const ieIndexBuffer& IndexBuffer) override;

				virtual void DrawMesh() override;

				virtual void TransitionResource(IGPUResource& Resource, EResourceState NewState, bool FlushImmediate = false) override;


				virtual UInt64 Flush(bool WaitForCompletion = false) override;
				virtual UInt64 Finish(bool WaitForCompletion = false) override;

				virtual void FlushResourceBarriers() override;

			private:
				D3D12CommandContext(const ECommandListType& Type);
				~D3D12CommandContext();

				D3D12_COMMAND_LIST_TYPE m_D3DCmdListType;
				UInt32 m_NumBarriersToFlush;
				D3D12_RESOURCE_BARRIER m_ResourceBarrierBuffer[16];

				ID3D12GraphicsCommandList* m_pID3D12CommandList;
				ID3D12CommandAllocator* m_pID3D12CurrentCmdAllocator;

			};
		}
	}
}
