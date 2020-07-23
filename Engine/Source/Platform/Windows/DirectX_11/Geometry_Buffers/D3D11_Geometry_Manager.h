#pragma once

#include <Insight/Core.h>

#include "Insight/Systems/Managers/Geometry_Manager.h"

namespace Insight {

	class INSIGHT_API D3D11GeometryManager : public GeometryManager
	{
	public:
		D3D11GeometryManager() = default;
		virtual ~D3D11GeometryManager();

		virtual bool InitImpl() override;
		virtual void RenderImpl(eRenderPass RenderPass) override;
		virtual void GatherGeometryImpl() override;
		virtual void PostRenderImpl() override;

	private:

	};

}
