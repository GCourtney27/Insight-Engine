#pragma once

#include <Runtime/CoreMacros.h>

#include "Runtime/Systems/Managers/GeometryManager.h"

namespace Insight {

	class INSIGHT_API D3D11GeometryManager : public GeometryManager
	{
		friend class GeometryManager;
	public:
		virtual bool Init_Impl() override;
		virtual void Render_Impl(RenderPassType RenderPass) override;
		virtual void GatherGeometry_Impl() override;

		virtual VertexBufferHandle CreateVertexBuffer_Impl() override;
		virtual IndexBufferHandle CreateIndexBuffer_Impl() override;

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
