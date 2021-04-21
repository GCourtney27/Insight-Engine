#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/GraphicsCore.h"

namespace Insight
{
	namespace Graphics
	{
		class ICommandContext;

		class INSIGHT_API IVertexBuffer
		{
			friend class IGeometryBufferManager;
		public:
			virtual ~IVertexBuffer() {}
			
			virtual void* GetNativeBufferView() = 0;

			virtual void Create(const EString& Name, UInt32 VertexDataSize, UInt32 VertexStrideSize, void* pVerticies) = 0;

		protected:
			IVertexBuffer() {}

			void SetUID(VertexBufferUID& NewUID) { m_UID = NewUID; }

			VertexBufferUID m_UID;
		};
	}
}
