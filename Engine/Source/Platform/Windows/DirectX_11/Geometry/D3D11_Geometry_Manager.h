#pragma once

#include <Insight/Core.h>

#include "Insight/Systems/Managers/Geometry_Manager.h"

namespace Insight {

	class INSIGHT_API D3D11GeometryManager : public GeometryManager
	{
		friend class GeometryManager;
	public:
		virtual bool InitImpl() override;
		virtual void RenderImpl(eRenderPass RenderPass) override;
		virtual void GatherGeometryImpl() override;
		virtual void PostRenderImpl() override;

	private:
		D3D11GeometryManager() = default;
		virtual ~D3D11GeometryManager();

	private:
		ID3D11Device* m_pDevice = nullptr;
		ID3D11DeviceContext* m_pDeviceContext = nullptr;

		ComPtr<ID3D11Buffer> m_pIntermediateConstantBuffer;

	};

}
