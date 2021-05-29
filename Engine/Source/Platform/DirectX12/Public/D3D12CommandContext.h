#pragma once

#include <Runtime/Core.h>
#include <Runtime/Graphics/Public/GraphicsCore.h>

#include "Runtime/Graphics/Public/ICommandContext.h"
#include "Runtime/Graphics/Public/ICommandManager.h"

#include "Platform/DirectX12/Private/D3D12DynamicDescriptorHeap.h"
#include <Runtime/Graphics/Public/IRootSignature.h>
#include "Platform/DirectX12/Private/LinearAllocator.h"


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
				D3D12ContextManager();
				virtual ~D3D12ContextManager();

				virtual ICommandContext* AllocateContext(ECommandListType Type) override;
				virtual void FreeContext(ICommandContext* pContext) override;
				virtual void DestroyAllContexts() override;
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

				virtual void OMSetRenderTargets(UInt32 NumRTVs, const IColorBuffer* Targets[], IDepthBuffer* pDepthBuffer) override;
				virtual void ClearDepth(IDepthBuffer& DepthBuffer) override;

				virtual void RSSetViewPorts(UInt32 NumViewPorts, const ViewPort* ViewPorts) override;
				virtual void RSSetScissorRects(UInt32 NumScissorRects, const Rect* ScissorRects) override;
				
				virtual void ClearColorBuffer(IColorBuffer& Buffer, Rect& Rect) override;

				virtual void CreateTexture2D() override;
				virtual void CreateBuffer() override;
				
				virtual void SetDescriptorHeap(EResourceHeapType Type, IDescriptorHeap* HeapPtr) override;

				DynAlloc ReserveUploadMemory(UInt64 SizeInBytes);
				virtual void UpdateSubresources(IGPUResource& Destination, IGPUResource& Intermediate, UInt32 IntermediateOffset, UInt32 FirstSubresource, UInt32 NumSubresources, SubResourceData& SubresourceData) override;

				virtual void SetDepthBufferAsTexture(UInt32 RootParameterIndex, const IDepthBuffer* pDepthBuffer) override;
				virtual void SetColorBuffersAsTextures(UInt32 RootParameterIndex, UInt32 Offset, UInt32 Count, const IColorBuffer* Buffers[]) override;
				virtual void SetColorBufferAsTexture(UInt32 RootParameterIndex, UInt32 Offset, IColorBuffer* Buffer) override;
				virtual void BindVertexBuffer(UInt32 Slot, IVertexBuffer& Vertexbuffer) override;
				virtual void BindIndexBuffer(IIndexBuffer& IndexBuffer) override;
				virtual void SetGraphicsConstantBuffer(UInt32 RootParameterIndex, IConstantBuffer* pConstantBuffer) override;
				virtual void SetTexture(UInt32 RootParameterIndex, ITextureRef& pTexture) override;

				virtual void SetPipelineState(IPipelineState& Pipeline) override;
				virtual void SetGraphicsRootSignature(IRootSignature& Signature) override;
				virtual void SetPrimitiveTopologyType(EPrimitiveTopology TopologyType) override;

				virtual void Draw(UInt32 VertexCount, UInt32 VertexStartOffset = 0) override;
				virtual void DrawIndexed(UInt32 IndexCount, UInt32 StartIndexLocation = 0, Int32 BaseVertexLocation = 0) override;
				virtual void DrawInstanced(UInt32 VertexCountPerInstance, UInt32 InstanceCount, UInt32 StartVertexLocation = 0, UInt32 StartInstanceLocation = 0) override;
				virtual void DrawIndexedInstanced(UInt32 IndexCountPerInstance, UInt32 InstanceCount, UINT StartIndexLocation, UInt32 BaseVertexLocation, UInt32 StartInstanceLocation) override;


				virtual void TransitionResource(IGPUResource& Resource, EResourceState NewState, bool FlushImmediate = false) override;


				virtual UInt64 Flush(bool WaitForCompletion = false) override;
				virtual UInt64 Finish(bool WaitForCompletion = false) override;

				virtual void FlushResourceBarriers() override;

			private:
				D3D12CommandContext(const ECommandListType& Type);
				~D3D12CommandContext();

				virtual void BindDescriptorHeaps() override;


				D3D12_COMMAND_LIST_TYPE m_D3DCmdListType;
				UInt32 m_NumBarriersToFlush;
				D3D12_RESOURCE_BARRIER m_ResourceBarrierBuffer[16];

				ID3D12GraphicsCommandList* m_pID3D12CommandList;
				ID3D12CommandAllocator* m_pID3D12CurrentCmdAllocator;

				ID3D12DescriptorHeap* m_CurrentDescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

				D3D12DynamicDescriptorHeap m_DynamicViewDescriptorHeap;		// HEAP_TYPE_CBV_SRV_UAV
				D3D12DynamicDescriptorHeap m_DynamicSamplerDescriptorHeap;	// HEAP_TYPE_SAMPLER

				LinearAllocator m_CpuLinearAllocator;
				LinearAllocator m_GpuLinearAllocator;
			};
		}
	}
}
