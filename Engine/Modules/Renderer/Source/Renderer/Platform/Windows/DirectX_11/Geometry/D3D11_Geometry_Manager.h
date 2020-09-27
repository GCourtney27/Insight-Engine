#pragma once

#include <Retina/Core.h>

#include "Retina/Systems/Managers/Geometry_Manager.h"

namespace Retina {

	class RETINA_API D3D11GeometryManager : public GeometryManager
	{
		friend class GeometryManager;
	public:
		virtual bool Init_Impl() override;
		virtual void Render_Impl(eRenderPass RenderPass) override;
		virtual void GatherGeometry_Impl() override;

	private:
		D3D11GeometryManager() = default;
		virtual ~D3D11GeometryManager();

	private:
		ID3D11Device* m_pDevice = nullptr;
		ID3D11DeviceContext* m_pDeviceContext = nullptr;

		ComPtr<ID3D11Buffer> m_pIntermediatePerObjectCB;
		ComPtr<ID3D11Buffer> m_pIntermediatematOverridesCB;

	};

}
