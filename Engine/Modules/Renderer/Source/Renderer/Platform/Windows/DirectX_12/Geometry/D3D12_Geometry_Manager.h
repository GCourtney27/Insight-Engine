#pragma once

#include <Insight/Core.h>

#include "Insight/Systems/Managers/Geometry_Manager.h"

namespace Insight {

	class INSIGHT_API D3D12GeometryManager : public GeometryManager
	{
		friend class GeometryManager;
	public:
		virtual bool Init_Impl() override;
		virtual void Render_Impl(eRenderPass RenderPass) override;
		virtual void GatherGeometry_Impl() override;

	private:
		D3D12GeometryManager() = default;
		virtual ~D3D12GeometryManager();
	private:
		D3D12_GPU_VIRTUAL_ADDRESS m_CbvUploadHeapHandle;
		D3D12_GPU_VIRTUAL_ADDRESS m_CbvMaterialHeapHandle;

		UINT8* m_CbvPerObjectGPUAddress = nullptr;
		UINT8* m_CbvMaterialGPUAddress = nullptr;

		ID3D12Resource* m_ConstantBufferUploadHeaps = nullptr;
		ID3D12Resource* m_ConstantBufferMaterialUploadHeaps = nullptr;
		ID3D12GraphicsCommandList* m_pScenePassCommandList = nullptr;
		ID3D12GraphicsCommandList* m_pShadowPassCommandList = nullptr;
		ID3D12GraphicsCommandList* m_pTransparencyPassCommandList = nullptr;

		int ConstantBufferPerObjectAlignedSize = (sizeof(CB_VS_PerObject) + 255) & ~255;
		int ConstantBufferPerObjectMaterialAlignedSize = (sizeof(CB_PS_VS_PerObjectMaterialAdditives) + 255) & ~255;
		UINT32 m_PerObjectCBDrawOffset = 0u;
		UINT32 m_GPUAddressUploadOffset = 0u;

	};

}
