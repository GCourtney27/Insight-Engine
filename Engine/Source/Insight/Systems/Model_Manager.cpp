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
		m_ConstantBufferUploadHeaps = &Direct3D12Context::Get().GetConstantBufferPerObjectUploadHeap();
		m_pCommandList = &Direct3D12Context::Get().GetCommandList();

		return (m_pCommandList && m_ConstantBufferUploadHeaps);
	}

	void ModelManager::Draw()
	{
		D3D12_GPU_VIRTUAL_ADDRESS cbvHandle(m_ConstantBufferUploadHeaps->GetGPUVirtualAddress());
		UINT32 constantBufferOffset = 0;
		for (unsigned int i = 0; i < m_Models.size(); ++i)
		{
			for (unsigned int j = 0; j < m_Models[i]->GetNumChildMeshes(); j++)
			{
				m_pCommandList->SetGraphicsRootConstantBufferView(0, cbvHandle + (ConstantBufferPerObjectAlignedSize * constantBufferOffset++));

				m_Models[i]->GetMeshAtIndex(j)->Render();
			}
		}

	}

	// Update the Constant buffers in the gpu with the new data for each model. Does not draw models
	void ModelManager::UploadVertexDataToGPU()
	{
		UINT8* cbvGPUAddress = &Direct3D12Context::Get().GetConstantBufferViewGPUHeapAddress();
		UINT32 gpuAdressOffset = 0;
		
		ACamera& camera = APlayerCharacter::Get().GetCameraRef();
		XMMATRIX viewMatTransposed = XMMatrixTranspose(camera.GetViewMatrix());
		XMMATRIX projectionMatTransposed = XMMatrixTranspose(camera.GetProjectionMatrix());
		XMFLOAT4X4 viewFloat;
		XMStoreFloat4x4(&viewFloat, viewMatTransposed);
		XMFLOAT4X4 projectionFloat;
		XMStoreFloat4x4(&projectionFloat, projectionMatTransposed);

		for (unsigned int i = 0; i < m_Models.size(); i++)
		{

			for (unsigned int j = 0; j < m_Models[i]->GetNumChildMeshes(); j++)
			{
				CB_VS_PerObject cbPerObject = m_Models[i]->GetMeshAtIndex(j)->GetConstantBuffer();
				cbPerObject.view = viewFloat;
				cbPerObject.projection = projectionFloat;
				memcpy(cbvGPUAddress + (ConstantBufferPerObjectAlignedSize * gpuAdressOffset++), &cbPerObject, sizeof(cbPerObject));
			}

		}

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

