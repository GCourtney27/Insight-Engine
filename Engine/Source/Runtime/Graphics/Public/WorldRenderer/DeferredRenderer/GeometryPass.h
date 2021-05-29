#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/WorldRenderer/IRenderPass.h"

namespace Insight
{
	class INSIGHT_API GeometryPass : public IRenderPass
	{
	public:
		GeometryPass() {}
		~GeometryPass() {}

		virtual void Begin() override;
		virtual void End() override;

	protected:
		virtual void Initialize() override;
		virtual void UnInitialize() override;
	};
}
