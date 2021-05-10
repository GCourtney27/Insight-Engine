#include <Engine_pch.h>

#include "Runtime/Core/Public/Layer/Layer.h"

namespace Insight {
	Layer::Layer(const std::string & name)
		: m_DebugName(name)
	{
	}

	Layer::~Layer()
	{
	}
}