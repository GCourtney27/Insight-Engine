#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/GraphicsCore.h"

namespace Insight
{
	namespace Graphics
	{

		class INSIGHT_API IGeometryManager
		{
		public:
			virtual VertexBufferUID AllocateVertexBuffer() = 0;
			virtual IndexBufferUID AllocateIndexBuffer() = 0;

			virtual void DeAllocateVertexBuffer(VertexBufferUID& UID) = 0;
			virtual void DeAllocateIndexBuffer(IndexBufferUID& UID) = 0;

			virtual IVertexBuffer& GetVertexBufferByUID(VertexBufferUID& UID) = 0;
			virtual IIndexBuffer& GetIndexBufferByUID(IndexBufferUID& UID) = 0;

		protected:
			IGeometryManager() {}
			~IGeometryManager() {}

			static VertexBufferUID s_NextVertexBufferID;
			static IndexBufferUID s_NextIndexBufferID;
		};
	}
}
