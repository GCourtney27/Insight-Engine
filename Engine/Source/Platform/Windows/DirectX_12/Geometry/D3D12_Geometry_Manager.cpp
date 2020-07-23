#include <ie_pch.h>

#include "D3D12_Geometry_Manager.h"

#include "Platform/Windows/DirectX_12/Direct3D12_Context.h"

#include "Insight/Rendering/Material.h"

namespace Insight {



	D3D12GeometryManager::~D3D12GeometryManager()
	{
	}

	bool D3D12GeometryManager::InitImpl()
	{
		Direct3D12Context* D3D12Context = reinterpret_cast<Direct3D12Context*>(&Renderer::Get());
		

		m_ConstantBufferUploadHeaps = &D3D12Context->GetConstantBufferPerObjectUploadHeap();
		m_ConstantBufferMaterialUploadHeaps = &D3D12Context->GetConstantBufferPerObjectMaterialUploadHeap();
		m_pScenePassCommandList = &D3D12Context->GetScenePassCommandList();
		m_pShadowPassCommandList = &D3D12Context->GetShadowPassCommandList();

		m_CbvUploadHeapHandle = m_ConstantBufferUploadHeaps->GetGPUVirtualAddress();
		m_CbvMaterialHeapHandle = m_ConstantBufferMaterialUploadHeaps->GetGPUVirtualAddress();

		m_CbvPerObjectGPUAddress = &D3D12Context->GetPerObjectCBVGPUHeapAddress();
		m_CbvMaterialGPUAddress = &D3D12Context->GetPerObjectMaterialAdditiveCBVGPUHeapAddress();

		if (!(m_pScenePassCommandList && m_pShadowPassCommandList && m_ConstantBufferUploadHeaps && m_ConstantBufferMaterialUploadHeaps))
		{
			IE_CORE_ERROR("Failed to initialize one or more resources for model manager.");
			return false;
		}
		return true;
	}

	void D3D12GeometryManager::RenderImpl(eRenderPass RenderPass)
	{
		if (RenderPass == eRenderPass::RenderPass_Shadow) {

			for (UINT32 i = 0; i < m_Models.size(); ++i) {

				if (m_Models[i]->GetCanCastShadows()) {

					for (UINT32 j = 0; j < m_Models[i]->GetNumChildMeshes(); j++) {

						// Set Per-Object CBV
						m_pShadowPassCommandList->SetGraphicsRootConstantBufferView(0, m_CbvUploadHeapHandle + (ConstantBufferPerObjectAlignedSize * m_PerObjectCBDrawOffset));
						m_Models[i]->GetMeshAtIndex(j)->Render(m_pShadowPassCommandList);

						m_PerObjectCBDrawOffset++;
					}
				}
			}
			m_PerObjectCBDrawOffset = 0U;
		}
		else if (RenderPass == eRenderPass::RenderPass_Scene) {

			for (UINT32 i = 0; i < m_Models.size(); ++i) {

				if (m_Models[i]->GetCanBeRendered()) {

					for (UINT32 j = 0; j < m_Models[i]->GetNumChildMeshes(); j++) {

						// Set Per-Object CBV
						m_pScenePassCommandList->SetGraphicsRootConstantBufferView(0, m_CbvUploadHeapHandle + (ConstantBufferPerObjectAlignedSize * m_PerObjectCBDrawOffset));
						// Set Per-Object Material Override CBV
						m_pScenePassCommandList->SetGraphicsRootConstantBufferView(4, m_CbvMaterialHeapHandle + (ConstantBufferPerObjectMaterialAlignedSize * m_PerObjectCBDrawOffset));

						m_Models[i]->BindResources();
						m_Models[i]->GetMeshAtIndex(j)->Render(m_pScenePassCommandList);

						m_PerObjectCBDrawOffset++;
					}
				}
			}
			m_PerObjectCBDrawOffset = 0U;
		}
	}

	void D3D12GeometryManager::GatherGeometryImpl()
	{
		for (UINT32 i = 0; i < m_Models.size(); i++) {

			if (m_Models[i]->GetCanBeRendered()) {

				for (UINT32 j = 0; j < m_Models[i]->GetNumChildMeshes(); j++) {

					CB_PS_VS_PerObjectAdditives cbMatOverrides = m_Models[i]->GetMaterialRef().GetMaterialOverrideConstantBuffer();
					memcpy(m_CbvMaterialGPUAddress + (ConstantBufferPerObjectMaterialAlignedSize * m_GPUAddressUploadOffset), &cbMatOverrides, sizeof(cbMatOverrides));

					CB_VS_PerObject cbPerObject = m_Models[i]->GetMeshAtIndex(j)->GetConstantBuffer();
					memcpy(m_CbvPerObjectGPUAddress + (ConstantBufferPerObjectAlignedSize * m_GPUAddressUploadOffset), &cbPerObject, sizeof(cbPerObject));

					m_GPUAddressUploadOffset++;
				}
			}
		}
	}

	void D3D12GeometryManager::PostRenderImpl()
	{
		m_PerObjectCBDrawOffset = 0u;
		m_GPUAddressUploadOffset = 0u;
	}

}

