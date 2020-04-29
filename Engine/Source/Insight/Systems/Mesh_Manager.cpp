#include "ie_pch.h"

#include "Mesh_Manager.h"

#include "Platform/DirectX12/Direct3D12_Context.h"

namespace Insight {



	ModelManager::ModelManager()
	{
	}

	ModelManager::~ModelManager()
	{
	}

	bool ModelManager::Init()
	{
		m_ConstantBufferUploadHeaps = &Direct3D12Context::Get().GetConstantBufferUploadHeap();
		return true;
	}

	void ModelManager::Draw()
	{
		for (unsigned int i = 0; i < m_Models.size(); i++)
		{

			for (unsigned int j = 0; j < m_Models[i]->GetNumChildMeshes(); j++)
			{
				Direct3D12Context::Get().GetCommandList().SetGraphicsRootConstantBufferView(0, m_ConstantBufferUploadHeaps->GetGPUVirtualAddress() + (ConstantBufferPerObjectAlignedSize * j));

				m_Models[i]->GetMeshAtIndex(j).Draw();
			}

		}
	}

	void ModelManager::Update()
	{
		UINT8* cbvGPUAddress = &Direct3D12Context::Get().GetConstantBufferViewGPUHeapAddress();
		for (unsigned int i = 0; i < m_Models.size(); i++)
		{

			for (unsigned int j = 0; j < m_Models[i]->GetNumChildMeshes(); j++)
			{
				CB_VS_PerObject cbPerObject = m_Models[i]->GetMeshAtIndex(j).GetConstantBuffer();
				
				memcpy(cbvGPUAddress + (ConstantBufferPerObjectAlignedSize * j), &cbPerObject, sizeof(cbPerObject));
			}

		}

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
		return true;
	}


}

