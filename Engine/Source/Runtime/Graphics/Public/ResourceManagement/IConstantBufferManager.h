#pragma once

#include "EngineDefines.h"
#include "InsightStd.h"
#include <Runtime/Graphics/Public/GraphicsCore.h>


namespace Insight
{
	namespace Graphics
	{
		class INSIGHT_API IConstantBuffer
		{
			friend class IConstantBufferManager;
		public:
			template <typename BufferCastType>
			FORCEINLINE BufferCastType* GetBufferPointer();
			FORCEINLINE ConstantBufferUID GetUID() const;
			FORCEINLINE UInt32 GetBufferSize() const;

		protected:
			IConstantBuffer()
				: m_BufferSize(0)
				, m_UID(IE_INVALID_CONSTANT_BUFFER_HANDLE)
			{
				ZeroMemRanged(m_Data, sizeof(UInt8) * IE_MAX_CONSTANT_BUFFER_SIZE);
			}
			~IConstantBuffer() = default;

			FORCEINLINE void SetBufferSize(UInt32 BufferSize);
			FORCEINLINE void SetUID(const ConstantBufferUID& UID);

			virtual void Create(const FString& Name, UInt32 BufferSize) = 0;

			UInt32 m_BufferSize;
			ConstantBufferUID m_UID;
			UInt8 m_Data[IE_MAX_CONSTANT_BUFFER_SIZE];
		};

		class INSIGHT_API IConstantBufferManager
		{
			friend class IRenderContext;
		public:
			virtual void CreateConstantBuffer(const FString& Name, IConstantBuffer** OutBuffer, UInt32 BufferSizeInBytes) = 0;
			virtual void DestroyConstantBuffer(ConstantBufferUID BufferHandle) = 0;

			virtual void Initialize() = 0;

		protected:
			IConstantBufferManager() 
			{
			}
			virtual ~IConstantBufferManager() 
			{
			}

			static ConstantBufferUID s_NextAvailableBufferID;
		};


		//
		// Inline function implementations
		//

		FORCEINLINE ConstantBufferUID IConstantBuffer::GetUID() const
		{ 
			return m_UID; 
		}
		
		FORCEINLINE UInt32 IConstantBuffer::GetBufferSize() const
		{ 
			return m_BufferSize; 
		}

		template <typename BufferCastType>
		FORCEINLINE BufferCastType* IConstantBuffer::GetBufferPointer()
		{
			return RCast<BufferCastType*>(m_Data);
		}

		FORCEINLINE void IConstantBuffer::SetBufferSize(UInt32 BufferSize)
		{ 
			m_BufferSize = BufferSize; 
		}
		
		FORCEINLINE void IConstantBuffer::SetUID(const ConstantBufferUID& UID)
		{ 
			m_UID = UID; 
		}
	}
}
