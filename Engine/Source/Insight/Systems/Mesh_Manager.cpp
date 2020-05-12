#include "ie_pch.h"

#include "Mesh_Manager.h"

#include "Platform/DirectX12/Direct3D12_Context.h"

namespace Insight {

	ModelManager* ModelManager::s_Instance = nullptr;

	ModelManager::ModelManager()
	{
		IE_CORE_ASSERT(!s_Instance, "An instance of Model Manager already exists!");
		s_Instance = this;
	}

	ModelManager::~ModelManager()
	{
		FlushModelCache();
	}

	bool ModelManager::Init()
	{
		m_ConstantBufferUploadHeaps = &Direct3D12Context::Get().GetConstantBufferUploadHeap();
		m_pCommandList = &Direct3D12Context::Get().GetCommandList();
		return true;
	}

	void ModelManager::Draw()
	{
		D3D12_GPU_VIRTUAL_ADDRESS cbvHandle(m_ConstantBufferUploadHeaps->GetGPUVirtualAddress());
		m_pCommandList->SetGraphicsRootConstantBufferView(0, cbvHandle);
		
		for (std::shared_ptr<Model>& model : m_Models)
		{
			for (unsigned int j = 0; j < model->GetNumChildMeshes(); j++)
			{
				model->GetMeshAtIndex(j).Draw();
			}
		}

	}

	// Update the Constant buffers in the gpu with the new data for each model. Does not draw models
	void ModelManager::UploadVertexDataToGPU()
	{
		UINT8* cbvGPUAddress = &Direct3D12Context::Get().GetConstantBufferViewGPUHeapAddress();
		UINT32 gpuAdressOffset = 0;
		XMMATRIX viewMat = Direct3D12Context::Get().GetCamera().GetViewMatrix();
		XMFLOAT4X4 viewFloat;
		XMStoreFloat4x4(&viewFloat, XMMatrixTranspose(viewMat));
		XMMATRIX projectionMat = Direct3D12Context::Get().GetCamera().GetProjectionMatrix();
		XMFLOAT4X4 projFloat;
		XMStoreFloat4x4(&projFloat, XMMatrixTranspose(projectionMat));

		for (unsigned int i = 0; i < m_Models.size(); i++)
		{

			for (unsigned int j = 0; j < m_Models[i]->GetNumChildMeshes(); j++)
			{
				// TODO: draw instanced if ref count is greater than one on shared pointer
				
				CB_VS_PerObject cbPerObject = m_Models[i]->GetMeshAtIndex(j).GetConstantBuffer();
				cbPerObject.view = viewFloat;
				cbPerObject.projection = projFloat;

				memcpy(cbvGPUAddress + (ConstantBufferPerObjectAlignedSize * gpuAdressOffset++), &cbPerObject, sizeof(cbPerObject));
			}

		}

		// for each model in m_Models map
		//		

	}

	bool ModelManager::LoadMeshFromFile(const std::string& filePath, bool async)
	{
		std::shared_ptr<Model> mesh = std::make_shared<Model>();
		if (!mesh->Init(filePath))
		{
			IE_CORE_ERROR("MeshManager::LoadMeshFromFile Failed to load mesh from file: {0}", filePath);
			return false;
		}
		m_Models.push_back(mesh);
		//m_Models.emplace(mesh, {Transform()});
		return true;
	}

	void ModelManager::FlushModelCache()
	{
		for (std::shared_ptr<Model>& model : m_Models)
		{
			model->Destroy();
			model.reset();
		}
		m_Models.clear();
	}


}

