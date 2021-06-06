#pragma once

#include "EngineDefines.h"

#include <Runtime/Graphics/Public/GraphicsCore.h>


namespace Insight
{
	namespace Graphics
	{
		class ICommandContext;

		class INSIGHT_API IIndexBuffer
		{
			friend class IGeometryBufferManager;
		public:
			virtual ~IIndexBuffer() {}

			virtual void* GetNativeBufferView() = 0;

			virtual void Create(const FString& Name, UInt32 IndexDataSize, void* pIndices) = 0;

		protected:
			IIndexBuffer() {}

			void SetUID(IndexBufferUID& NewUID) { m_UID = NewUID; }

			IndexBufferUID m_UID;
		};
	}
}
