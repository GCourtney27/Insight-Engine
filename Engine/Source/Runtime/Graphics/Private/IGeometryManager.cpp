#include <Engine_pch.h>

#include "Runtime/Graphics/Public/IGeometryManager.h"


namespace Insight
{
	namespace Graphics
	{
		VertexBufferUID IGeometryManager::s_NextVertexBufferID = 0u;
		IndexBufferUID IGeometryManager::s_NextIndexBufferID = 0u;

	}
}
