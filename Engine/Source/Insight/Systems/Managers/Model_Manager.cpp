#include <ie_pch.h>

#include "Model_Manager.h"

#include "Platform/Windows/DirectX12/Direct3D12_Context.h"
#include "Insight/Runtime/APlayer_Character.h"
#include <fstream>

namespace Insight {

	ModelManager::ModelManager()
	{

	}

	ModelManager::~ModelManager()
	{
		FlushModelCache();
	}

	bool ModelManager::Init()
	{
		Direct3D12Context& GraphicsContext = Direct3D12Context::Get();

		m_ConstantBufferUploadHeaps = &GraphicsContext.GetConstantBufferPerObjectUploadHeap();
		m_ConstantBufferMaterialUploadHeaps = &GraphicsContext.GetConstantBufferPerObjectMaterialUploadHeap();
		m_pScenePassCommandList = &GraphicsContext.GetScenePassCommandList();
		m_pShadowPassCommandList = &GraphicsContext.GetShadowPassCommandList();

		m_CbvUploadHeapHandle = m_ConstantBufferUploadHeaps->GetGPUVirtualAddress();
		m_CbvMaterialHeapHandle = m_ConstantBufferMaterialUploadHeaps->GetGPUVirtualAddress();

		m_CbvPerObjectGPUAddress = &GraphicsContext.GetPerObjectCBVGPUHeapAddress();
		m_CbvMaterialGPUAddress = &GraphicsContext.GetPerObjectMaterialAdditiveCBVGPUHeapAddress();

		if (!(m_pScenePassCommandList && m_pShadowPassCommandList && m_ConstantBufferUploadHeaps && m_ConstantBufferMaterialUploadHeaps))
		{
			IE_CORE_ERROR("Failed to initialize one or more resources for model manager.");
			return false;
		}

		return true;
	}

	bool ModelManager::LoadResourcesFromJson(const rapidjson::Value& jsonMeshes)
	{
		return false;
	}

	// Issue draw commands to all models attached to the model manager
	void ModelManager::Render(RenderPass RenderPass)
	{
		if (RenderPass == RenderPass::RenderPass_Shadow) {

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
		else if (RenderPass == RenderPass::RenderPass_Scene) {

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

	// Update the Constant buffers in the gpu with the new data for each model. Does not draw models
	void ModelManager::GatherGeometry()
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

	void ModelManager::PostRender()
	{
		m_PerObjectCBDrawOffset = 0u;
		m_GPUAddressUploadOffset = 0u;
	}

	void ModelManager::FlushModelCache()
	{
		m_Models.clear();
	}

}

