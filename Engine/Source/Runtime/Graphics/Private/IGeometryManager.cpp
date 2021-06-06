#include <Engine_pch.h>

#include "Graphics/Public/IGeometryBufferManager.h"


namespace Insight
{
	namespace Graphics
	{
		VertexBufferUID IGeometryBufferManager::s_NextVertexBufferID = 0u;
		IndexBufferUID IGeometryBufferManager::s_NextIndexBufferID = 0u;

	}
}
