#include <ie_pch.h>

#include "Texture.h"

#include "Insight/Core/Application.h"
#include "Insight/Utilities/String_Helper.h"
#include "Platform/Windows/DirectX_12/Direct3D12_Context.h"

#define CBVSRV_HEAP_TEXTURE_START 7

namespace Insight {

	uint32_t Texture::s_NumSceneTextures = 0U;

	Texture::Texture(IE_TEXTURE_INFO createInfo, CDescriptorHeapWrapper& srvHeapHandle)
	{
		Init(createInfo, srvHeapHandle);
	}

	Texture::Texture(Texture&& texture) noexcept
	{
		m_pTexture = texture.m_pTexture;
		m_TextureDesc = texture.m_TextureDesc;
		m_TextureInfo = texture.m_TextureInfo;
		m_GPUHeapIndex = texture.m_GPUHeapIndex;
	
		texture.m_pTexture = nullptr;

		texture.m_TextureDesc = {};
		texture.m_GPUHeapIndex = 0u;

		m_pCommandList = nullptr;
	}

	Texture::~Texture()
	{
		Destroy();
	}

	void Texture::Destroy()
	{
		COM_SAFE_RELEASE(m_pTexture);
		m_pCommandList = nullptr;
	}

	bool Texture::Init(IE_TEXTURE_INFO createInfo, CDescriptorHeapWrapper& srvHeapHandle)
	{
		Direct3D12Context& graphicsContext = Direct3D12Context::Get();
		std::string filepath = StringHelper::WideToString(createInfo.Filepath);
		m_pCbvSrvHeapStart = &graphicsContext.GetCBVSRVDescriptorHeap();
		m_pCommandList = &graphicsContext.GetScenePassCommandList();
		m_TextureInfo = createInfo;
		m_TextureInfo.DisplayName = StringHelper::GetFilenameFromDirectory(filepath);

		std::string fileExtension = StringHelper::GetFileExtension(filepath);
		if (fileExtension == "dds") {
			InitDDSTexture(srvHeapHandle);
		}
		else {
			InitTextureFromFile(srvHeapHandle);
		}

		m_RootParamIndex = GetRootParameterIndexForTextureType(m_TextureInfo.Type);
		return true;
	}

	bool Texture::InitTextureFromFile(CDescriptorHeapWrapper& srvHeapHandle)
	{		
		Direct3D12Context& graphicsContext = Direct3D12Context::Get();
		ID3D12Device* pDevice = &graphicsContext.GetDeviceContext();
		ID3D12CommandQueue* pCommandQueue = &graphicsContext.GetCommandQueue();

		ResourceUploadBatch resourceUpload(pDevice);
		resourceUpload.Begin();

		ThrowIfFailed(CreateWICTextureFromFile(pDevice, resourceUpload, m_TextureInfo.Filepath.c_str(), &m_pTexture, m_TextureInfo.GenerateMipMaps), "Failed to Create WIC texture from file.");
		m_TextureDesc = m_pTexture->GetDesc();
		if (!resourceUpload.IsSupportedForGenerateMips(m_TextureDesc.Format)) {
			//IE_CORE_WARN("Mip map generation not supported for texture: {0}", m_DisplayName);
		}

		// Upload the resources to the GPU
		auto uploadResourcesFinished = resourceUpload.End(pCommandQueue);

		// Wait for the upload thread to terminate
		uploadResourcesFinished.wait();

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Texture2D.MipLevels = m_TextureDesc.MipLevels;
		srvDesc.Format = m_TextureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		pDevice->CreateShaderResourceView(m_pTexture.Get(), &srvDesc, srvHeapHandle.hCPU(CBVSRV_HEAP_TEXTURE_START + s_NumSceneTextures));

		m_GPUHeapIndex = s_NumSceneTextures;
		s_NumSceneTextures++;
		return true;
	}

	void Texture::InitDDSTexture(CDescriptorHeapWrapper& srvHeapHandle)
	{
		Direct3D12Context& GraphicsContext = Direct3D12Context::Get();
		ID3D12Device* pDevice = &GraphicsContext.GetDeviceContext();
		ID3D12CommandQueue* pCommandQueue = &GraphicsContext.GetCommandQueue();
		
		ResourceUploadBatch ResourceUpload(pDevice);
		ResourceUpload.Begin();

		HRESULT hr;
		hr = CreateDDSTextureFromFile(pDevice, ResourceUpload, m_TextureInfo.Filepath.c_str(), &m_pTexture, m_TextureInfo.GenerateMipMaps, 0, nullptr, &m_TextureInfo.IsCubeMap);
		ThrowIfFailed(hr, "Failed to load DDS texture from file");
		m_TextureDesc = m_pTexture->GetDesc();
		if (!ResourceUpload.IsSupportedForGenerateMips(m_TextureDesc.Format)) {
			//IE_CORE_WARN("Mip map generation not supported for texture: {0}", m_DisplayName);
		}

		// Upload the resources to the GPU
		auto UploadResourcesFinished = ResourceUpload.End(pCommandQueue);

		// Wait for the upload thread to terminate
		UploadResourcesFinished.wait();

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Texture2D.MipLevels = m_TextureDesc.MipLevels;
		srvDesc.Format = m_TextureDesc.Format;
		// Regular dds texture or a cubemap?
		srvDesc.ViewDimension = (m_TextureInfo.Type >= eTextureType::SKY_IRRADIENCE) ? D3D12_SRV_DIMENSION_TEXTURECUBE : D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		pDevice->CreateShaderResourceView(m_pTexture.Get(), &srvDesc, srvHeapHandle.hCPU(CBVSRV_HEAP_TEXTURE_START + s_NumSceneTextures));

		m_GPUHeapIndex = s_NumSceneTextures;
		s_NumSceneTextures++;
	}

	void Texture::Bind()
	{
		m_pCommandList->SetGraphicsRootDescriptorTable(m_RootParamIndex, m_pCbvSrvHeapStart->hGPU(CBVSRV_HEAP_TEXTURE_START + m_GPUHeapIndex));
	}

	UINT Texture::GetRootParameterIndexForTextureType(eTextureType TextureType)
	{
		switch (m_TextureInfo.Type) {
		case eTextureType::ALBEDO:
		{
			return 6;
			break;
		}
		case eTextureType::NORMAL:
		{
			return 7;
			break;
		}
		case eTextureType::ROUGHNESS:
		{
			return 8;
			break;
		}
		case eTextureType::METALLIC:
		{
			return 9;
			break;
		}
		case eTextureType::AO:
		{
			return 10;
			break;
		}
		case eTextureType::SKY_IRRADIENCE:
		{
			return 12;
			break;
		}
		case eTextureType::SKY_ENVIRONMENT_MAP:
		{
			return 13;
			break;
		}
		case eTextureType::SKY_BRDF_LUT:
		{
			return 14;
			break;
		}
		case eTextureType::SKY_DIFFUSE:
		{
			return 15;
			break;
		}
		default:
		{
			IE_CORE_WARN("Failed to get root parameter index with texture type: {0}", TextureType);
			break;
		}
		}
		return 6;
	}
}