#include <ie_pch.h>

#include "ie_D3D12_Texture.h"

#include "Insight/Systems/File_System.h"
#include "Platform/Windows/DirectX_12/Direct3D12_Context.h"

#include <DirectX12/TK/Inc/DDSTextureLoader.h>
#include <DirectX12/TK/Inc/WICTextureLoader.h>
#include "DirectX12/TK/Inc/ResourceUploadBatch.h"

#define CBVSRV_HEAP_TEXTURE_START_SLOT 7


namespace Insight {


	uint32_t ieD3D12Texture::s_NumSceneTextures = 0U;

	ieD3D12Texture::ieD3D12Texture(IE_TEXTURE_INFO CreateInfo, CDescriptorHeapWrapper& srvHeapHandle)
		: Texture(CreateInfo)
	{
		Init(CreateInfo, srvHeapHandle);
	}

	ieD3D12Texture::~ieD3D12Texture()
	{
	}

	void ieD3D12Texture::Destroy()
	{
		COM_SAFE_RELEASE(m_pTexture);
		m_pCommandList = nullptr;
	}

	void ieD3D12Texture::Bind()
	{
		m_pCommandList->SetGraphicsRootDescriptorTable(m_RootParamIndex, m_pCbvSrvHeapStart->hGPU(CBVSRV_HEAP_TEXTURE_START_SLOT + m_GPUHeapIndex));
	}

	bool ieD3D12Texture::Init(IE_TEXTURE_INFO createInfo, CDescriptorHeapWrapper& srvHeapHandle)
	{
		Direct3D12Context* GraphicsContext = reinterpret_cast<Direct3D12Context*>(&Renderer::Get());
		std::string Filepath = StringHelper::WideToString(createInfo.Filepath);

		m_pCbvSrvHeapStart = &GraphicsContext->GetCBVSRVDescriptorHeap();
		m_pCommandList = &GraphicsContext->GetScenePassCommandList();
		m_TextureInfo = createInfo;
		m_TextureInfo.DisplayName = StringHelper::GetFilenameFromDirectory(Filepath);

		std::string FileExtension = StringHelper::GetFileExtension(Filepath);
		if (FileExtension == "dds") {
			InitDDSTexture(srvHeapHandle);
		}
		else {
			InitTextureFromFile(srvHeapHandle);
		}

		m_RootParamIndex = GetRootParameterIndexForTextureType(m_TextureInfo.Type);
		return true;
	}

	void ieD3D12Texture::InitDDSTexture(CDescriptorHeapWrapper& srvHeapHandle)
	{
		Direct3D12Context* GraphicsContext = reinterpret_cast<Direct3D12Context*>(&Renderer::Get());
		ID3D12Device* pDevice = &GraphicsContext->GetDeviceContext();
		ID3D12CommandQueue* pCommandQueue = &GraphicsContext->GetCommandQueue();

		DirectX::ResourceUploadBatch ResourceUpload(pDevice);
		ResourceUpload.Begin();

		HRESULT hr;
		hr = DirectX::CreateDDSTextureFromFile(pDevice, ResourceUpload, m_TextureInfo.Filepath.c_str(), &m_pTexture, m_TextureInfo.GenerateMipMaps, 0, nullptr, &m_TextureInfo.IsCubeMap);
		if (FAILED(hr)) {
			IE_CORE_ERROR("Failed to load DDS texture from file with path \"{0}\"", StringHelper::WideToString(m_TextureInfo.Filepath));
		}

		m_D3DTextureDesc = m_pTexture->GetDesc();
		if (!ResourceUpload.IsSupportedForGenerateMips(m_D3DTextureDesc.Format)) {
			//IE_CORE_WARN("Mip map generation not supported for texture: {0}", m_DisplayName);
		}

		// Upload the resources to the GPU
		auto UploadResourcesFinished = ResourceUpload.End(pCommandQueue);

		// Wait for the upload thread to terminate
		UploadResourcesFinished.wait();

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Texture2D.MipLevels = m_D3DTextureDesc.MipLevels;
		srvDesc.Format = m_D3DTextureDesc.Format;
		// Regular dds texture or a cubemap?
		srvDesc.ViewDimension = (m_TextureInfo.Type >= eTextureType::eTextureType_SkyIrradience) ? D3D12_SRV_DIMENSION_TEXTURECUBE : D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		pDevice->CreateShaderResourceView(m_pTexture.Get(), &srvDesc, srvHeapHandle.hCPU(CBVSRV_HEAP_TEXTURE_START_SLOT + s_NumSceneTextures));

		m_GPUHeapIndex = s_NumSceneTextures;
		s_NumSceneTextures++;
	}

	bool ieD3D12Texture::InitTextureFromFile(CDescriptorHeapWrapper& srvHeapHandle)
	{
		Direct3D12Context* graphicsContext = reinterpret_cast<Direct3D12Context*>(&Renderer::Get());
		ID3D12Device* pDevice = &graphicsContext->GetDeviceContext();
		ID3D12CommandQueue* pCommandQueue = &graphicsContext->GetCommandQueue();

		DirectX::ResourceUploadBatch resourceUpload(pDevice);
		resourceUpload.Begin();

		HRESULT hr = DirectX::CreateWICTextureFromFile(pDevice, resourceUpload, m_TextureInfo.Filepath.c_str(), &m_pTexture, m_TextureInfo.GenerateMipMaps);
		if (FAILED(hr)) {
			IE_CORE_ERROR("Failed to Create WIC texture from file with path \"{0}\"", StringHelper::WideToString(m_TextureInfo.Filepath));
		}
		m_D3DTextureDesc = m_pTexture->GetDesc();
		if (!resourceUpload.IsSupportedForGenerateMips(m_D3DTextureDesc.Format)) {
			//IE_CORE_WARN("Mip map generation not supported for texture: {0}", m_DisplayName);
		}

		// Upload the resources to the GPU
		auto uploadResourcesFinished = resourceUpload.End(pCommandQueue);

		// Wait for the upload thread to terminate
		uploadResourcesFinished.wait();

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Texture2D.MipLevels = m_D3DTextureDesc.MipLevels;
		srvDesc.Format = m_D3DTextureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		pDevice->CreateShaderResourceView(m_pTexture.Get(), &srvDesc, srvHeapHandle.hCPU(CBVSRV_HEAP_TEXTURE_START_SLOT + s_NumSceneTextures));

		m_GPUHeapIndex = s_NumSceneTextures;
		s_NumSceneTextures++;
		return true;
	}

	UINT ieD3D12Texture::GetRootParameterIndexForTextureType(eTextureType TextureType)
	{
		switch (m_TextureInfo.Type) {
		case eTextureType::eTextureType_Albedo:
		{
			return 6;
			break;
		}
		case eTextureType::eTextureType_Normal:
		{
			return 7;
			break;
		}
		case eTextureType::eTextureType_Roughness:
		{
			return 8;
			break;
		}
		case eTextureType::eTextureType_Metallic:
		{
			return 9;
			break;
		}
		case eTextureType::eTextureType_AmbientOcclusion:
		{
			return 10;
			break;
		}
		case eTextureType::eTextureType_SkyIrradience:
		{
			return 12;
			break;
		}
		case eTextureType::eTextureType_SkyEnvironmentMap:
		{
			return 13;
			break;
		}
		case eTextureType::eTextureType_IBLBRDFLUT:
		{
			return 14;
			break;
		}
		case eTextureType::eTextureType_SkyDiffuse:
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
