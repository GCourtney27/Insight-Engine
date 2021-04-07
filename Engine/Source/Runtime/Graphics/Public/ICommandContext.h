#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/CommonEnums.h"

namespace Insight
{
	namespace Graphics
	{
		class ICommandContext;
		class ICommandManager;
		class IColorBuffer;
		class IGPUResource;

		struct ViewPort;
		struct Rect;

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

			static ICommandContext& Begin(const EString& ID);
			
			virtual UInt64 Flush(bool WaitForCompletion = false) = 0;
			virtual UInt64 Finish(bool WaitForCompletion = false) = 0;

			virtual void RSSetViewPorts(UInt32 NumViewPorts, const ViewPort* ViewPorts) = 0;
			virtual void RSSetScissorRects(UInt32 NumScissorRects, const Rect* ScissorRects) = 0;

			virtual void ClearColorBuffer(IColorBuffer& Buffer, Rect& Rect) = 0;

			virtual void CreateTexture2D() = 0;
			virtual void CreateBuffer() = 0;

			virtual void BindVertexBuffer(const ieVertexBuffer& Vertexbuffer) = 0;
			virtual void BindIndexBuffer(const ieIndexBuffer& IndexBuffer) = 0;

			virtual void DrawMesh() = 0;

			virtual void TransitionResource(IGPUResource& Resource, EResourceState NewState, bool FlushImmediate = false) = 0;

			virtual void FlushResourceBarriers() = 0;

			void SetID(const EString& NewID) { m_ID = NewID; }

		protected:
			ICommandContext(const ECommandListType& Type) 
				: m_ID()
				, m_Type(Type)
			{
			}
			virtual ~ICommandContext() 
			{
			}

			EString m_ID;
			ECommandListType m_Type;
		};
	}
}
