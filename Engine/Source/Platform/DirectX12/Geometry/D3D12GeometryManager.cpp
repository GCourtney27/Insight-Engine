#include <Engine_pch.h>

#include "D3D12GeometryManager.h"

#include "Platform/DirectX12/Direct3D12Context.h"

#include "Runtime/Rendering/Material.h"

namespace Insight {



	D3D12GeometryManager::~D3D12GeometryManager()
	{
	}

	D3D12GeometryManager::VertexBufferHandle D3D12GeometryManager::CreateVertexBuffer_Impl()
	{
		return -1;
	}

	D3D12GeometryManager::IndexBufferHandle D3D12GeometryManager::CreateIndexBuffer_Impl()
	{
		return -1;
	}

	bool D3D12GeometryManager::Init_Impl()
	{
		Direct3D12Context& RenderContext = Renderer::GetAs<Direct3D12Context>();

		m_ConstantBufferUploadHeaps = &RenderContext.GetConstantBufferPerObjectUploadHeap();
		m_ConstantBufferMaterialUploadHeaps = &RenderContext.GetConstantBufferPerObjectMaterialUploadHeap();
		m_pScenePassCommandList = &RenderContext.GetScenePassCommandList();
		m_pShadowPassCommandList = &RenderContext.GetShadowPassCommandList();
		m_pTransparencyPassCommandList = &RenderContext.GetTransparencyPassCommandList();

		m_CbvUploadHeapHandle = m_ConstantBufferUploadHeaps->GetGPUVirtualAddress();
		m_CbvMaterialHeapHandle = m_ConstantBufferMaterialUploadHeaps->GetGPUVirtualAddress();

		m_CbvPerObjectGPUAddress = RenderContext.GetPerObjectCBVGPUHeapAddress();
		m_CbvMaterialGPUAddress = RenderContext.GetPerObjectMaterialAdditiveCBVGPUHeapAddress();

		if (!(m_pScenePassCommandList && m_pShadowPassCommandList && m_pTransparencyPassCommandList && m_ConstantBufferUploadHeaps && m_ConstantBufferMaterialUploadHeaps))
		{
			IE_LOG(Error, TEXT("Failed to initialize one or more resources for D3D12 model manager."));
			return false;
		}
		return true;
	}

	void D3D12GeometryManager::Render_Impl(RenderPassType RenderPass)
	{
		if (RenderPass == RenderPassType::RenderPassType_Shadow) {

			for (UINT32 i = 0; i < m_OpaqueModels.size(); ++i) {

				if (m_OpaqueModels[i]->GetCanCastShadows()) {

					for (UINT32 j = 0; j < m_OpaqueModels[i]->GetNumChildMeshes(); j++) {

						// Set Per-Object CBV
						// We dont need any texture of material data,
						// we only want the depth for the shadow map for each oject
						m_pShadowPassCommandList->SetGraphicsRootConstantBufferView(0, m_CbvUploadHeapHandle + (ConstantBufferPerObjectAlignedSize * m_PerObjectCBDrawOffset));
						m_OpaqueModels[i]->GetMeshAtIndex(j)->Render();

						m_PerObjectCBDrawOffset++;
					}
				}
			}
		}
		else if (RenderPass == RenderPassType::RenderPassType_Scene) {

			for (UINT32 i = 0; i < m_OpaqueModels.size(); ++i) {

				if (m_OpaqueModels[i]->GetCanBeRendered()) {

					for (UINT32 j = 0; j < m_OpaqueModels[i]->GetNumChildMeshes(); j++) {

						// Set Per-Object CBV
						m_pScenePassCommandList->SetGraphicsRootConstantBufferView(0, m_CbvUploadHeapHandle + (ConstantBufferPerObjectAlignedSize * m_PerObjectCBDrawOffset));
						// Set Per-Object Material Override CBV
						m_pScenePassCommandList->SetGraphicsRootConstantBufferView(4, m_CbvMaterialHeapHandle + (ConstantBufferPerObjectMaterialAlignedSize * m_PerObjectCBDrawOffset));

						m_OpaqueModels[i]->BindResources(true);
						m_OpaqueModels[i]->GetMeshAtIndex(j)->Render();

						m_PerObjectCBDrawOffset++;
					}
				}
			}
		}
		else if (RenderPass == RenderPassType::RenderPassType_Transparency) {

			for (UINT32 i = 0; i < m_TranslucentModels.size(); ++i) {

				if (m_TranslucentModels[i]->GetCanBeRendered()) {

					for (UINT32 j = 0; j < m_TranslucentModels[i]->GetNumChildMeshes(); j++) {

						// Set Per-Object CBV
						m_pTransparencyPassCommandList->SetGraphicsRootConstantBufferView(0, m_CbvUploadHeapHandle + (ConstantBufferPerObjectAlignedSize * m_PerObjectCBDrawOffset));
						// Set Per-Object Material Override CBV
						m_pTransparencyPassCommandList->SetGraphicsRootConstantBufferView(3, m_CbvMaterialHeapHandle + (ConstantBufferPerObjectMaterialAlignedSize * m_PerObjectCBDrawOffset));

						m_TranslucentModels[i]->BindResources(false);
						m_TranslucentModels[i]->GetMeshAtIndex(j)->Render();

						m_PerObjectCBDrawOffset++;
					}
				}
			}
		}
		m_PerObjectCBDrawOffset = 0U;
	}

	void D3D12GeometryManager::GatherGeometry_Impl()
	{

		for (UINT32 i = 0; i < m_OpaqueModels.size(); i++) {

			if (m_OpaqueModels[i]->GetCanBeRendered()) {

				for (UINT32 j = 0; j < m_OpaqueModels[i]->GetNumChildMeshes(); j++) {

					const CB_PS_VS_PerObjectMaterialAdditives cbMatOverrides = m_OpaqueModels[i]->GetMaterialRef().GetMaterialOverrideConstantBuffer();
					memcpy(m_CbvMaterialGPUAddress + (ConstantBufferPerObjectMaterialAlignedSize * m_GPUAddressUploadOffset), &cbMatOverrides, sizeof(cbMatOverrides));

					const CB_VS_PerObject cbPerObject = m_OpaqueModels[i]->GetMeshAtIndex(j)->GetConstantBuffer();
					memcpy(m_CbvPerObjectGPUAddress + (ConstantBufferPerObjectAlignedSize * m_GPUAddressUploadOffset), &cbPerObject, sizeof(cbPerObject));

					m_GPUAddressUploadOffset++;
				}
			}
		}

		for (UINT32 i = 0; i < m_TranslucentModels.size(); i++) {

			if (m_TranslucentModels[i]->GetCanBeRendered()) {

				for (UINT32 j = 0; j < m_TranslucentModels[i]->GetNumChildMeshes(); j++) {

					const CB_PS_VS_PerObjectMaterialAdditives cbMatOverrides = m_TranslucentModels[i]->GetMaterialRef().GetMaterialOverrideConstantBuffer();
					memcpy(m_CbvMaterialGPUAddress + (ConstantBufferPerObjectMaterialAlignedSize * m_GPUAddressUploadOffset), &cbMatOverrides, sizeof(cbMatOverrides));

					const CB_VS_PerObject cbPerObject = m_TranslucentModels[i]->GetMeshAtIndex(j)->GetConstantBuffer();
					memcpy(m_CbvPerObjectGPUAddress + (ConstantBufferPerObjectAlignedSize * m_GPUAddressUploadOffset), &cbPerObject, sizeof(cbPerObject));

					m_GPUAddressUploadOffset++;
				}
			}
		}
		m_GPUAddressUploadOffset = 0u;
	}

}

