#pragma once

#include "Runtime/Core.h"

namespace Insight
{
	namespace Graphics
	{
		class IDevice;

		enum ECommandQueueType
		{
			CQT_Direct,
			CQT_Compute,
			CQT_Copy,
			CQT_Bundle,
		};

#if IE_PLATFORM_WINDOWS
		FORCE_INLINE constexpr D3D12_COMMAND_LIST_TYPE IECommandQueueTypeToD3DCommandQueueType(const ECommandQueueType& Type)
		{
			switch (Type)
			{
			case ECommandQueueType::CQT_Direct: return D3D12_COMMAND_LIST_TYPE_DIRECT;
			case ECommandQueueType::CQT_Compute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
			default:
				IE_LOG(Error, TEXT("Failed to convert command queue type with specified enum value to D3D12 command queue type!"));
				return D3D12_COMMAND_LIST_TYPE_DIRECT;
			}
		}
#endif

		class INSIGHT_API ICommandQueue
		{
			friend class ICommandManager;
		public:
			virtual void* GetNativeQueue() = 0;

		protected:
			ICommandQueue(const ECommandQueueType& Type)
				: m_Type(Type)
			{
			}
			~ICommandQueue()
			{
			}
			
			ECommandQueueType m_Type;
		};

		class INSIGHT_API ICommandManager
		{
			friend class D3D12ContextFactory;
			friend class IRenderContext;
		public:
			virtual void Initialize(IDevice* pDevice) = 0;


		protected:
			ICommandManager() 
				: m_pDevice(NULL)
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
				m_pDevice = NULL;

				SAFE_DELETE_PTR(m_pGraphicsQueue);
				SAFE_DELETE_PTR(m_pComputeQueue);
			}


			inline ICommandQueue* GetGraphicsQueue() const { return m_pGraphicsQueue; }
			inline ICommandQueue* GetComputeQueue() const { return m_pComputeQueue; }

			inline void SetGraphicsQueue(ICommandQueue* pCommandQueue) { m_pGraphicsQueue = pCommandQueue; }

			IDevice* m_pDevice;
			ICommandQueue* m_pGraphicsQueue;
			ICommandQueue* m_pComputeQueue;
		};
	}
}
