#pragma once

#include "EngineDefines.h"

namespace Insight
{
	class INSIGHT_API IRenderPass
	{
	public:
		virtual void Begin() = 0;
		virtual void End() = 0;

	protected:
		virtual void Initialize() = 0;
		virtual void UnInitialize() = 0;
	};
}
