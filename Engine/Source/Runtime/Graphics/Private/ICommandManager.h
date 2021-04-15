#pragma once

#include "Runtime/Core.h"

#include "Runtime/Graphics/Public/CommonEnums.h"

namespace Insight
{
	namespace Graphics
	{
		class IDevice;
		class ICommandContext;
		class ICommandAllocator;

		class INSIGHT_API ICommandQueue
		{
			friend class ICommandManager;
		public:
			virtual void* GetNativeQueue() = 0;

			virtual void WaitforFence(UInt64 FenceValue) = 0;
			virtual bool IsFenceCompleted(UInt64 FenceValue) = 0;

		protected:
			ICommandQueue(const ECommandListType& Type)
				: m_Type(Type)
			{
			}
			virtual ~ICommandQueue()
			{
			}
			
			ECommandListType m_Type;
		};

		class INSIGHT_API ICommandManager
		{
			friend class D3D12RenderContextFactory;
			friend class D3D12CommandManager;
			friend class IRenderContext;
		public:
			virtual void Initialize(IDevice* pDevice) = 0;

			virtual void CreateNewCommandContext(const ECommandListType& Type, ICommandContext** pContext, void** pData) = 0;

			virtual void WaitForFence(UInt64 Value) = 0;

			inline ICommandQueue* GetQueue(ECommandListType Type)
			{
				switch (Type)
				{
				case ECommandListType::CLT_Direct: return m_pGraphicsQueue;
				case ECommandListType::CLT_Compute: return m_pComputeQueue;
				default:
					IE_LOG(Warning, TEXT("Unidentified enum value provided when getting command queue: %i"), Type);
					return (ICommandQueue*)(NULL);
				}
			}
			inline ICommandQueue* GetGraphicsQueue()	const { return m_pGraphicsQueue; }
			inline ICommandQueue* GetComputeQueue()		const { return m_pComputeQueue; }

			// Test to see if a fence has already been reached
			inline bool IsFenceComplete(uint64_t FenceValue)
			{
				return GetQueue(ECommandListType(FenceValue >> 56))->IsFenceCompleted(FenceValue);
			}

		protected:
			ICommandManager() 
				: m_pDeviceRef(NULL)
				, m_pGraphicsQueue(NULL)
				, m_pComputeQueue(NULL)
			{
			}
			virtual ~ICommandManager() 
			{
				UnInitialize();
			}

			virtual void UnInitialize()
			{
				m_pDeviceRef = NULL;

				m_pGraphicsQueue = NULL;
				m_pComputeQueue	= NULL;
			}

			inline void SetGraphicsQueue(ICommandQueue* pCommandQueue) { m_pGraphicsQueue = pCommandQueue; }

			IDevice* m_pDeviceRef;

			// Implementations of this interface control the lifetime of these objects.
			//
			ICommandQueue* m_pGraphicsQueue;
			ICommandQueue* m_pComputeQueue;
		};
	}
}
