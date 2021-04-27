#include <Engine_pch.h>

#include "Runtime/Graphics/Public/ResourceManagement/IConstantBufferManager.h"

namespace Insight
{
	namespace Graphics
	{
		ConstantBufferUID IConstantBufferManager::s_NextAvailableBufferID = 0;

	}
}