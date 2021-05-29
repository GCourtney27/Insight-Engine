#pragma once

#include <Runtime/Core.h>
#include <Runtime/Graphics/Public/GraphicsCore.h>


namespace Insight
{
	namespace Graphics
	{

		constexpr int cx_ContextPoolSize = 4;
		constexpr int cx_AvailableContextPoolSize = cx_ContextPoolSize;

		/*
			Manages command context lifetime and creation.
		*/
		class INSIGHT_API IContextManager
		{
			friend class IRenderContext;
		public:
			virtual ICommandContext* AllocateContext(ECommandListType Type) = 0;
			virtual void FreeContext(ICommandContext* pContext) = 0;
			virtual void DestroyAllContexts() = 0;

		protected:
			IContextManager();
			virtual ~IContextManager();

			std::vector<ICommandContext*> m_ContextPool[cx_ContextPoolSize];
			std::queue<ICommandContext*> m_AvailableContexts[cx_AvailableContextPoolSize];
			std::mutex m_ContextAllocationMutex;
		};

		/*
			Command reciever that executes commands.
		*/
		class INSIGHT_API ICommandContext
		{
			friend class D3D12CommandManager;
			friend IContextManager;
		public:
			virtual void* GetNativeContext() = 0;
			virtual void** GetNativeContextAddress() = 0;

			virtual void Initialize() = 0;
			virtual void UnInitialize() = 0;
			virtual void Reset() = 0;

			static ICommandContext& Begin(const FString& ID);
			
			virtual UInt64 Flush(bool WaitForCompletion = false) = 0;
			virtual UInt64 Finish(bool WaitForCompletion = false) = 0;

			virtual void OMSetRenderTargets(UInt32 NumRTVs, const IColorBuffer* Targets[], IDepthBuffer* pDepthBuffer) = 0;

			virtual void RSSetViewPorts(UInt32 NumViewPorts, const ViewPort* ViewPorts) = 0;
			virtual void RSSetScissorRects(UInt32 NumScissorRects, const Rect* ScissorRects) = 0;

			virtual void ClearColorBuffer(IColorBuffer& Buffer, Rect& Rect) = 0;
			virtual void ClearDepth(IDepthBuffer& DepthBuffer) = 0;

			virtual void CreateTexture2D() = 0;
			virtual void CreateBuffer() = 0;
			
			virtual void SetDescriptorHeap(EResourceHeapType Type, IDescriptorHeap* HeapPtr) = 0;

			virtual void UpdateSubresources(IGPUResource& Destination, IGPUResource& Intermediate, UInt32 IntermediateOffset, UInt32 FirstSubresource, UInt32 NumSubresources, SubResourceData& SubresourceData) = 0;

			virtual void SetDepthBufferAsTexture(UInt32 RootParameterIndex, const IDepthBuffer* pDepthBuffer) = 0;
			virtual void SetColorBuffersAsTextures(UInt32 RootParameterIndex, UInt32 Offset, UInt32 Count, const IColorBuffer* Buffers[]) = 0;
			virtual void SetColorBufferAsTexture(UInt32 RootParameterIndex, UInt32 Offset, IColorBuffer* Buffer) = 0;
			virtual void BindVertexBuffer(UInt32 Slot, IVertexBuffer& Vertexbuffer) = 0;
			virtual void BindIndexBuffer(IIndexBuffer& IndexBuffer) = 0;
			virtual void SetGraphicsConstantBuffer(UInt32 RootParameterIndex, IConstantBuffer* pConstantBuffer) = 0;
			virtual void SetTexture(UInt32 Slot, ITextureRef& pTexture) = 0;

			virtual void SetPipelineState(IPipelineState& Pipeline) = 0;
			virtual void SetGraphicsRootSignature(IRootSignature& Signature) = 0;
			virtual void SetPrimitiveTopologyType(EPrimitiveTopology TopologyType) = 0;

			virtual void Draw(UInt32 VertexCount, UInt32 VertexStartOffset) = 0;
			virtual void DrawIndexed(UInt32 IndexCount, UInt32 StartIndexLocation, Int32 BaseVertexLocation) = 0;
			virtual void DrawInstanced(UInt32 VertexCountPerInstance, UInt32 InstanceCount, UInt32 StartVertexLocation, UInt32 StartInstanceLocation) = 0;
			virtual void DrawIndexedInstanced(UInt32 IndexCountPerInstance, UInt32 InstanceCount, UINT StartIndexLocation, UInt32 BaseVertexLocation, UInt32 StartInstanceLocation) = 0;

			virtual void TransitionResource(IGPUResource& Resource, EResourceState NewState, bool FlushImmediate = false) = 0;

			virtual void FlushResourceBarriers() = 0;

			void SetID(const FString& NewID) { m_ID = NewID; }

		protected:
			ICommandContext(const ECommandListType& Type) 
				: m_ID()
				, m_Type(Type)
			{
			}
			virtual ~ICommandContext() 
			{
			}

			virtual void BindDescriptorHeaps() = 0;

			FString m_ID;
			ECommandListType m_Type;
		};
	}
}
