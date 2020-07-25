#include <ie_pch.h>

#include "D3D11_Geometry_Manager.h"

#include "Platform/Windows/DirectX_11/Direct3D11_Context.h"


namespace Insight {


	D3D11GeometryManager::~D3D11GeometryManager()
	{
	}

	bool D3D11GeometryManager::InitImpl()
	{
		Direct3D11Context* D3D12Context = reinterpret_cast<Direct3D11Context*>(&Renderer::Get());
		m_pDevice = &D3D12Context->GetDevice();
		m_pDeviceContext = &D3D12Context->GetDeviceContext();

		D3D11_BUFFER_DESC BufferDesc = {};
		BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		BufferDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		BufferDesc.MiscFlags = 0U;
		BufferDesc.ByteWidth = static_cast<UINT>(sizeof(CB_VS_PerObject) + (16 - sizeof(CB_VS_PerObject) % 16));
		BufferDesc.StructureByteStride = 0U;
		HRESULT hr = m_pDevice->CreateBuffer(&BufferDesc, nullptr, m_pIntermediateConstantBuffer.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create intermediate Per-Object constant buffer for D3D 11 context.");

		return true;
	}

	void D3D11GeometryManager::RenderImpl(eRenderPass RenderPass)
	{
		HRESULT hr;
		
		for (UINT32 i = 0; i < s_Instance->m_Models.size(); ++i) {

			if (s_Instance->m_Models[i]->GetCanBeRendered()) {

				for (UINT32 j = 0; j < s_Instance->m_Models[i]->GetNumChildMeshes(); j++) {

					CB_VS_PerObject cbPerObject = s_Instance->m_Models[i]->GetMeshAtIndex(j)->GetConstantBuffer();
					
					D3D11_MAPPED_SUBRESOURCE PerObjectMappedResource = {};
					hr = m_pDeviceContext->Map(m_pIntermediateConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &PerObjectMappedResource);
					CopyMemory(PerObjectMappedResource.pData, &cbPerObject, sizeof(CB_VS_PerObject));
					m_pDeviceContext->Unmap(m_pIntermediateConstantBuffer.Get(), 0);
					m_pDeviceContext->VSSetConstantBuffers(0, 1, m_pIntermediateConstantBuffer.GetAddressOf());


					s_Instance->m_Models[i]->BindResources();
					s_Instance->m_Models[i]->GetMeshAtIndex(j)->Render(nullptr);
				}
			}
		}
	}

	void D3D11GeometryManager::GatherGeometryImpl()
	{
	}

	void D3D11GeometryManager::PostRenderImpl()
	{
	}

}
