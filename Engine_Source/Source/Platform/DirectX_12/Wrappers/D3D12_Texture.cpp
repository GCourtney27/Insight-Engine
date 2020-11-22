#include <Engine_pch.h>

#include "D3D12_Texture.h"

#include "Platform/DirectX_12/Direct3D12_Context.h"

#include <DirectX12/TK/Inc/DDSTextureLoader.h>
#include <DirectX12/TK/Inc/WICTextureLoader.h>
#include <DirectX12/TK/Inc/ResourceUploadBatch.h>

#define CBVSRV_HEAP_TEXTURE_START_SLOT 14					// Keep this in sync with Direct3D12Context::m_cbvsrvHeap
#define OBJECT_TEXTURE_DEF_PASS_ROOT_PARAM_INDEX_START 6	// Keep this in sync with deferred shading pass root signature
#define OBJECT_TEXTURE_FORW_PASS_ROOT_PARAM_INDEX_START 4	// Keep this in sync with forward shading pass root signature
#define OBJECT_TEXTURE_FORW_DEFF_ROOT_PARAM_INDEX_DIFF (OBJECT_TEXTURE_DEF_PASS_ROOT_PARAM_INDEX_START - OBJECT_TEXTURE_FORW_PASS_ROOT_PARAM_INDEX_START)

namespace Insight {


	uint32_t ieD3D12Texture::s_NumSceneTextures = 0U;

	ieD3D12Texture::ieD3D12Texture(IE_TEXTURE_INFO CreateInfo, CDescriptorHeapWrapper& srvHeapHandle)
		: Texture(CreateInfo),
		m_pScenePass_CommandList(nullptr),
		m_pTranslucencyPass_CommandList(nullptr),
		m_GPUHeapIndex(0U),
		m_RootParamIndex(0U)
	{
		Init(CreateInfo, srvHeapHandle);
	}

	ieD3D12Texture::~ieD3D12Texture()
	{
	}

	void ieD3D12Texture::Destroy()
	{
		COM_SAFE_RELEASE(m_pTexture);
		m_pScenePass_CommandList = nullptr;
		m_pTranslucencyPass_CommandList = nullptr;
	}

	void ieD3D12Texture::BindForDeferredPass()
	{
		m_pScenePass_CommandList->SetGraphicsRootDescriptorTable(m_RootParamIndex, m_pCbvSrvHeapStart->hGPU(CBVSRV_HEAP_TEXTURE_START_SLOT + m_GPUHeapIndex));
	}

	void ieD3D12Texture::BindForForwardPass()
	{
		m_pTranslucencyPass_CommandList->SetGraphicsRootDescriptorTable(m_RootParamIndex, m_pCbvSrvHeapStart->hGPU(CBVSRV_HEAP_TEXTURE_START_SLOT + m_GPUHeapIndex));
	}

	bool ieD3D12Texture::Init(IE_TEXTURE_INFO createInfo, CDescriptorHeapWrapper& srvHeapHandle)
	{
		std::string Filepath = StringHelper::WideToString(createInfo.Filepath);
		Direct3D12Context& RenderContext = Renderer::GetAs<Direct3D12Context>();


		m_pCbvSrvHeapStart = &RenderContext.GetCBVSRVDescriptorHeap();
		m_pScenePass_CommandList = &RenderContext.GetScenePassCommandList();
		m_pTranslucencyPass_CommandList = &RenderContext.GetTransparencyPassCommandList();
		m_TextureInfo = createInfo;

		std::string FileExtension = StringHelper::GetFileExtension(Filepath);
		if (FileExtension == "dds") {
			InitDDSTexture(srvHeapHandle);
		}
		else if (FileExtension == "hdr") {
			InitHDRTexture(srvHeapHandle);
		}
		else {
			InitTextureFromFile(srvHeapHandle);
		}

		m_RootParamIndex = GetRootParameterIndexForTextureType(m_TextureInfo.Type);
		return true;
	}

	void ieD3D12Texture::InitDDSTexture(CDescriptorHeapWrapper& srvHeapHandle)
	{
		Direct3D12Context& RenderContext = Renderer::GetAs<Direct3D12Context>();
		ID3D12Device* pDevice = &RenderContext.GetDeviceContext();

		ID3D12CommandQueue* pCommandQueue = &RenderContext.GetCommandQueue();

		DirectX::ResourceUploadBatch ResourceUpload(pDevice);
		ResourceUpload.Begin();

		HRESULT hr;
		hr = DirectX::CreateDDSTextureFromFile(pDevice, ResourceUpload, m_TextureInfo.Filepath.c_str(), &m_pTexture, m_TextureInfo.GenerateMipMaps, 0, nullptr, &m_TextureInfo.IsCubeMap);
		if (FAILED(hr)) {
			IE_DEBUG_LOG(LogSeverity::Error, "Failed to load DDS texture from file with path \"{0}\"", StringHelper::WideToString(m_TextureInfo.Filepath));
		}

		m_D3DTextureDesc = m_pTexture->GetDesc();
		if (!ResourceUpload.IsSupportedForGenerateMips(m_D3DTextureDesc.Format)) {
			//IE_DEBUG_LOG(LogSeverity::Warning, "Mip map generation not supported for texture: {0}", m_DisplayName);
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

	void ieD3D12Texture::InitHDRTexture(CDescriptorHeapWrapper& srvHeapHandle)
	{

	}

	bool ieD3D12Texture::InitTextureFromFile(CDescriptorHeapWrapper& srvHeapHandle)
	{
		Direct3D12Context& RenderContext = Renderer::GetAs<Direct3D12Context>();
		ID3D12Device* pDevice = &RenderContext.GetDeviceContext();

		ID3D12CommandQueue* pCommandQueue = &RenderContext.GetCommandQueue();

		DirectX::ResourceUploadBatch resourceUpload(pDevice);
		resourceUpload.Begin();

		HRESULT hr = S_OK;
#if defined (IE_PLATFORM_BUILD_WIN32)
		hr = DirectX::CreateWICTextureFromFile(pDevice, resourceUpload, m_TextureInfo.Filepath.c_str(), &m_pTexture, m_TextureInfo.GenerateMipMaps);

#elif defined (IE_PLATFORM_BUILD_UWP)

		//auto tempFolder = winrt::Windows::Storage::ApplicationData::Current().TemporaryFolder();
		auto tempFolder = winrt::Windows::ApplicationModel::Package::Current().InstalledLocation();
		winrt::hstring name = m_TextureInfo.Filepath.c_str();
		//auto future = tempFolder.GetFileAsync(L"../../Content/Textures/BambooWood/BambooWood_Albedo.png");
		auto future = tempFolder.GetFileAsync(m_TextureInfo.Filepath.c_str());
		if(future.get())
			hr = DirectX::CreateWICTextureFromFile(pDevice, resourceUpload, future.get().Path().c_str(), &m_pTexture, m_TextureInfo.GenerateMipMaps);

#endif

		if (FAILED(hr)) {
			IE_DEBUG_LOG(LogSeverity::Error, "Failed to Create WIC texture from file with path \"{0}\"", StringHelper::WideToString(m_TextureInfo.Filepath));
			return false;
		}

		m_D3DTextureDesc = m_pTexture->GetDesc();
		if (!resourceUpload.IsSupportedForGenerateMips(m_D3DTextureDesc.Format)) {
			//IE_DEBUG_LOG(LogSeverity::Warning, "Mip map generation not supported for texture: {0}", m_DisplayName);
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
			return OBJECT_TEXTURE_DEF_PASS_ROOT_PARAM_INDEX_START;
			break;
		}
		case eTextureType::eTextureType_Normal:
		{
			return OBJECT_TEXTURE_DEF_PASS_ROOT_PARAM_INDEX_START + 1;
			break;
		}
		case eTextureType::eTextureType_Roughness:
		{
			return OBJECT_TEXTURE_DEF_PASS_ROOT_PARAM_INDEX_START + 2;
			break;
		}
		case eTextureType::eTextureType_Metallic:
		{
			return OBJECT_TEXTURE_DEF_PASS_ROOT_PARAM_INDEX_START + 3;
			break;
		}
		case eTextureType::eTextureType_Opacity:
		{
			return OBJECT_TEXTURE_DEF_PASS_ROOT_PARAM_INDEX_START + 3;
			break;
		}
		case eTextureType::eTextureType_AmbientOcclusion:
		{
			return OBJECT_TEXTURE_DEF_PASS_ROOT_PARAM_INDEX_START + 4;
			break;
		}
		case eTextureType::eTextureType_Translucency:
		{
			return OBJECT_TEXTURE_DEF_PASS_ROOT_PARAM_INDEX_START + 4;
			break;
		}
		case eTextureType::eTextureType_SkyIrradience:
		{
			return OBJECT_TEXTURE_DEF_PASS_ROOT_PARAM_INDEX_START + 6;
			break;
		}
		case eTextureType::eTextureType_SkyRadianceMap:
		{
			return OBJECT_TEXTURE_DEF_PASS_ROOT_PARAM_INDEX_START + 7;
			break;
		}
		case eTextureType::eTextureType_IBLBRDFLUT:
		{
			return OBJECT_TEXTURE_DEF_PASS_ROOT_PARAM_INDEX_START + 8;
			break;
		}
		case eTextureType::eTextureType_SkyDiffuse:
		{
			return OBJECT_TEXTURE_DEF_PASS_ROOT_PARAM_INDEX_START + 9;
			break;
		}
		default:
		{
			IE_DEBUG_LOG(LogSeverity::Warning, "Failed to get root parameter index with texture type: {0}", TextureType);
			break;
		}
		}
		return 6;
	}

}
