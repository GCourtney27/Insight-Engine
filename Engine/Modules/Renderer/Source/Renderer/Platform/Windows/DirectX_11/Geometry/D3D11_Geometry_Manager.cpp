#include <Renderer_pch.h>

#include "D3D11_Geometry_Manager.h"

#include "Platform/Windows/DirectX_11/Direct3D11_Context.h"
#include "Insight/Rendering/Material.h"

namespace Insight {


	D3D11GeometryManager::~D3D11GeometryManager()
	{
	}

	bool D3D11GeometryManager::Init_Impl()
	{
		Direct3D11Context& RenderContext = Renderer::GetAs<Direct3D11Context>();
		m_pDevice = &RenderContext.GetDevice();

		m_pDeviceContext = &RenderContext.GetDeviceContext();

		D3D11_BUFFER_DESC PerObjectBufferDesc = {};
		PerObjectBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		PerObjectBufferDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
		PerObjectBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		PerObjectBufferDesc.MiscFlags = 0U;
		PerObjectBufferDesc.ByteWidth = static_cast<UINT>(sizeof(CB_VS_PerObject) + (16 - sizeof(CB_VS_PerObject) % 16));
		PerObjectBufferDesc.StructureByteStride = 0U;
		HRESULT hr = m_pDevice->CreateBuffer(&PerObjectBufferDesc, nullptr, m_pIntermediatePerObjectCB.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create intermediate Per-Object constant buffer for D3D 11 context.");

		D3D11_BUFFER_DESC MatOverridesBufferDesc = {};
		MatOverridesBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		MatOverridesBufferDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
		MatOverridesBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		MatOverridesBufferDesc.MiscFlags = 0U;
		MatOverridesBufferDesc.ByteWidth = static_cast<UINT>(sizeof(CB_PS_VS_PerObjectMaterialAdditives) + (16 - sizeof(CB_PS_VS_PerObjectMaterialAdditives) % 16));
		MatOverridesBufferDesc.StructureByteStride = 0U;
		hr = m_pDevice->CreateBuffer(&MatOverridesBufferDesc, nullptr, m_pIntermediatematOverridesCB.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create intermediate Per-Object material override constant buffer for D3D 11 context.");



		return true;
	}

	void D3D11GeometryManager::Render_Impl(RenderPassType RenderPass)
	{
		HRESULT hr;

		if (RenderPass == RenderPassType::RenderPassType_Scene) {

			for (UINT32 i = 0; i < s_Instance->m_OpaqueModels.size(); ++i) {

				if (s_Instance->m_OpaqueModels[i]->GetCanBeRendered()) {

					for (UINT32 j = 0; j < s_Instance->m_OpaqueModels[i]->GetNumChildMeshes(); j++) {

						CB_VS_PerObject cbPerObject = s_Instance->m_OpaqueModels[i]->GetMeshAtIndex(j)->GetConstantBuffer();
						D3D11_MAPPED_SUBRESOURCE PerObjectMappedResource = {};
						hr = m_pDeviceContext->Map(m_pIntermediatePerObjectCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &PerObjectMappedResource);
						CopyMemory(PerObjectMappedResource.pData, &cbPerObject, sizeof(CB_VS_PerObject));
						m_pDeviceContext->Unmap(m_pIntermediatePerObjectCB.Get(), 0);
						m_pDeviceContext->VSSetConstantBuffers(0, 1, m_pIntermediatePerObjectCB.GetAddressOf());

						CB_PS_VS_PerObjectMaterialAdditives cbMatOverrides = s_Instance->m_OpaqueModels[i]->GetMaterialRef().GetMaterialOverrideConstantBuffer();
						D3D11_MAPPED_SUBRESOURCE MatOverridesMappedResource = {};
						hr = m_pDeviceContext->Map(m_pIntermediatematOverridesCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MatOverridesMappedResource);
						CopyMemory(MatOverridesMappedResource.pData, &cbMatOverrides, sizeof(CB_PS_VS_PerObjectMaterialAdditives));
						m_pDeviceContext->Unmap(m_pIntermediatematOverridesCB.Get(), 0);
						m_pDeviceContext->VSSetConstantBuffers(4, 1, m_pIntermediatematOverridesCB.GetAddressOf());
						m_pDeviceContext->PSSetConstantBuffers(4, 1, m_pIntermediatematOverridesCB.GetAddressOf());


						s_Instance->m_OpaqueModels[i]->BindResources(true);
						s_Instance->m_OpaqueModels[i]->GetMeshAtIndex(j)->Render();
					}
				}
			}

		}
		else if (RenderPass == RenderPassType::RenderPassType_Transparency) {

			for (UINT32 i = 0; i < m_TranslucentModels.size(); ++i) {

				if (m_TranslucentModels[i]->GetCanBeRendered()) {

					for (UINT32 j = 0; j < m_TranslucentModels[i]->GetNumChildMeshes(); j++) {

						// Set Per-Object CBV
						CB_VS_PerObject cbPerObject = s_Instance->m_TranslucentModels[i]->GetMeshAtIndex(j)->GetConstantBuffer();
						D3D11_MAPPED_SUBRESOURCE PerObjectMappedResource = {};
						hr = m_pDeviceContext->Map(m_pIntermediatePerObjectCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &PerObjectMappedResource);
						CopyMemory(PerObjectMappedResource.pData, &cbPerObject, sizeof(CB_VS_PerObject));
						m_pDeviceContext->Unmap(m_pIntermediatePerObjectCB.Get(), 0);
						m_pDeviceContext->VSSetConstantBuffers(0, 1, m_pIntermediatePerObjectCB.GetAddressOf());

						// Set Per-Object Material Override CBV
						CB_PS_VS_PerObjectMaterialAdditives cbMatOverrides = s_Instance->m_TranslucentModels[i]->GetMaterialRef().GetMaterialOverrideConstantBuffer();
						D3D11_MAPPED_SUBRESOURCE MatOverridesMappedResource = {};
						hr = m_pDeviceContext->Map(m_pIntermediatematOverridesCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MatOverridesMappedResource);
						CopyMemory(MatOverridesMappedResource.pData, &cbMatOverrides, sizeof(CB_PS_VS_PerObjectMaterialAdditives));
						m_pDeviceContext->Unmap(m_pIntermediatematOverridesCB.Get(), 0);
						m_pDeviceContext->VSSetConstantBuffers(3, 1, m_pIntermediatematOverridesCB.GetAddressOf());
						m_pDeviceContext->PSSetConstantBuffers(3, 1, m_pIntermediatematOverridesCB.GetAddressOf());

						m_TranslucentModels[i]->BindResources(false);
						m_TranslucentModels[i]->GetMeshAtIndex(j)->Render();
					}
				}
			}
		}

	}

	void D3D11GeometryManager::GatherGeometry_Impl()
	{
	}

}
