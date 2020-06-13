#include <ie_pch.h>

#include "Model_Manager.h"

#include "Platform/DirectX12/Direct3D12_Context.h"
#include "Insight/Runtime/APlayer_Character.h"
#include <fstream>

namespace Insight {

	ModelManager* ModelManager::s_Instance = nullptr;

	ModelManager::ModelManager()
	{
		IE_CORE_ASSERT(!s_Instance, "An instance of Model Manager already exists!");
		s_Instance = this;
	}

	ModelManager::~ModelManager()
	{
		//FlushModelCache();
	}

	bool ModelManager::Init()
	{
		Direct3D12Context& graphicsContext = Direct3D12Context::Get();

		m_ConstantBufferUploadHeaps = &graphicsContext.GetConstantBufferPerObjectUploadHeap();
		m_ConstantBufferMaterialUploadHeaps = &graphicsContext.GetConstantBufferPerObjectMaterialUploadHeap();
		m_pCommandList = &graphicsContext.GetCommandList();

		m_CbvUploadHeapHandle = m_ConstantBufferUploadHeaps->GetGPUVirtualAddress();
		m_CbvMaterialHeapHandle = m_ConstantBufferMaterialUploadHeaps->GetGPUVirtualAddress();

		m_CbvPerObjectGPUAddress = &graphicsContext.GetPerObjectCBVGPUHeapAddress();
		m_CbvMaterialGPUAddress = &graphicsContext.GetPerObjectMaterialAdditiveCBVGPUHeapAddress();

		if (!(m_pCommandList && m_ConstantBufferUploadHeaps && m_ConstantBufferMaterialUploadHeaps))
		{
			IE_CORE_ERROR("Failed to initialize one or more resources for model manager.");
			return false;
		}

		return true;
	}

	// Issue draw commands to all models attached to the model manager
	void ModelManager::Render()
	{
		for (UINT32 i = 0; i < m_Models.size(); ++i) {

			for (UINT32 j = 0; j < m_Models[i]->GetNumChildMeshes(); j++) {

				m_pCommandList->SetGraphicsRootConstantBufferView(0, m_CbvUploadHeapHandle + (ConstantBufferPerObjectAlignedSize * m_PerObjectCBDrawOffset));
				m_pCommandList->SetGraphicsRootConstantBufferView(4, m_CbvMaterialHeapHandle + (ConstantBufferPerObjectMaterialAlignedSize * m_PerObjectCBDrawOffset));

				m_Models[i]->BindResources();
				m_Models[i]->GetMeshAtIndex(j)->Render();

				m_PerObjectCBDrawOffset++;
			}
		}
	}

	// Update the Constant buffers in the gpu with the new data for each model. Does not draw models
	void ModelManager::UploadVertexDataToGPU()
	{
		for (UINT32 i = 0; i < m_Models.size(); i++) {

			for (UINT32 j = 0; j < m_Models[i]->GetNumChildMeshes(); j++) {

				CB_PS_VS_PerObjectAdditives cbMatOverrides = m_Models[i]->GetMaterialRef().GetMaterialOverrideConstantBuffer();
				memcpy(m_CbvMaterialGPUAddress + (ConstantBufferPerObjectMaterialAlignedSize * m_GPUAddressUploadOffset), &cbMatOverrides, sizeof(cbMatOverrides));

				CB_VS_PerObject cbPerObject = m_Models[i]->GetMeshAtIndex(j)->GetConstantBuffer();
				memcpy(m_CbvPerObjectGPUAddress + (ConstantBufferPerObjectAlignedSize * m_GPUAddressUploadOffset), &cbPerObject, sizeof(cbPerObject));

				m_GPUAddressUploadOffset++;
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
		/*for (std::shared_ptr<Model>& model : m_Models)
		{
			model->Destroy();
			model.reset();

		}
		m_Models.clear();*/
	}

}

