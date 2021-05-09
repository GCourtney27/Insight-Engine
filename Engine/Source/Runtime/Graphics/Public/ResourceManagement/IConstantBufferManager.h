#pragma once

#include <Runtime/Core.h>
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
			FORCE_INLINE BufferCastType* GetBufferPointer();
			FORCE_INLINE ConstantBufferUID GetUID() const;
			FORCE_INLINE UInt32 GetBufferSize() const;

		protected:
			IConstantBuffer()
				: m_BufferSize(0)
				, m_UID(IE_INVALID_CONSTANT_BUFFER_HANDLE)
			{
				ZeroMem(m_Data, sizeof(char) * IE_MAX_CONSTANT_BUFFER_SIZE);
			}
			~IConstantBuffer() = default;

			FORCE_INLINE void SetBufferSize(UInt32 BufferSize);
			FORCE_INLINE void SetUID(const ConstantBufferUID& UID);

			virtual void Create(const EString& Name, UInt32 BufferSize) = 0;

			UInt32 m_BufferSize;
			ConstantBufferUID m_UID;
			char m_Data[IE_MAX_CONSTANT_BUFFER_SIZE];
		};

		class INSIGHT_API IConstantBufferManager
		{
		public:
			virtual void CreateConstantBuffer(const EString& Name, IConstantBuffer** OutBuffer, UInt32 BufferSizeInBytes) = 0;
			virtual void DestroyConstantBuffer(ConstantBufferUID BufferHandle) = 0;

			virtual void Initialize() = 0;

		protected:
			IConstantBufferManager() {}
			virtual ~IConstantBufferManager() {}

			static ConstantBufferUID s_NextAvailableBufferID;
		};


		//
		// Inline function implementations
		//

		FORCE_INLINE ConstantBufferUID IConstantBuffer::GetUID() const
		{ 
			return m_UID; 
		}
		
		FORCE_INLINE UInt32 IConstantBuffer::GetBufferSize() const
		{ 
			return m_BufferSize; 
		}

		template <typename BufferCastType>
		FORCE_INLINE BufferCastType* IConstantBuffer::GetBufferPointer()
		{
			return RCast<BufferCastType*>(m_Data);
		}

		FORCE_INLINE void IConstantBuffer::SetBufferSize(UInt32 BufferSize)
		{ 
			m_BufferSize = BufferSize; 
		}
		
		FORCE_INLINE void IConstantBuffer::SetUID(const ConstantBufferUID& UID)
		{ 
			m_UID = UID; 
		}
	}
}
