#include <Engine_pch.h>

#include "Texture_Manager.h"
#include "Insight/Systems/File_System.h"
#include "Insight/Utilities/String_Helper.h"
#include "Renderer/Renderer.h"

#include "Renderer/Platform/Windows/DirectX_12/Direct3D12_Context.h"
#include "Renderer/Platform/Windows/DirectX_12/ie_D3D12_Texture.h"
#include "Renderer/Platform/Windows/DirectX_11/ie_D3D11_Texture.h"

namespace Insight {

	TextureManager::TextureManager()
	{
	}

	TextureManager::~TextureManager()
	{
		Destroy();
	}

	void TextureManager::Destroy()
	{
	}

	void TextureManager::FlushTextureCache()
	{
		for (StrongTexturePtr& tex : m_AlbedoTextures) {
			tex.reset();
		}
		for (StrongTexturePtr& tex : m_NormalTextures) {
			tex.reset();
		}
		for (StrongTexturePtr& tex : m_MetallicTextures) {
			tex.reset();
		}
		for (StrongTexturePtr& tex : m_RoughnessTextures) {
			tex.reset();
		}
		for (StrongTexturePtr& tex : m_AOTextures) {
			tex.reset();
		}
		for (StrongTexturePtr& tex : m_OpacityTextures) {
			tex.reset();
		}
		for (StrongTexturePtr& tex : m_TranslucencyTextures) {
			tex.reset();
		}

	}

	bool TextureManager::Init()
	{
		LoadDefaultTextures();
		return true;
	}

	bool TextureManager::LoadResourcesFromJson(const rapidjson::Value& JsonTextures)
	{
		// Load each texture from the texture resource file and cache it.
		for (rapidjson::SizeType i = 0; i < JsonTextures.Size(); i++) {

			std::string Name, Filepath;
			int Type, ID;
			bool GenMipMaps;
			json::get_int(JsonTextures[i], "ID", ID);
			json::get_int(JsonTextures[i], "Type", Type);
			json::get_string(JsonTextures[i], "Name", Name);
			json::get_string(JsonTextures[i], "Filepath", Filepath);
			json::get_bool(JsonTextures[i], "GenerateMipMaps", GenMipMaps);

			Texture::IE_TEXTURE_INFO TexInfo = {};
			TexInfo.DisplayName = Name;
			TexInfo.Id = ID;
			TexInfo.Filepath = StringHelper::StringToWide(FileSystem::GetProjectRelativeAssetDirectory(Filepath));
			TexInfo.GenerateMipMaps = GenMipMaps;
			TexInfo.Type = (Texture::eTextureType)Type;

			m_Futures.push_back(std::async(std::launch::async, &TextureManager::RegisterTextureByType, this, TexInfo));
			//RegisterTextureByType(TexInfo);

			m_HighestTextureId = ((int)m_HighestTextureId < ID) ? ID : m_HighestTextureId;
		}

		return true;
	}

	StrongTexturePtr TextureManager::GetTextureByID(Texture::ID textureID, Texture::eTextureType textreType)
	{
		switch (textreType) {
		case Texture::eTextureType::eTextureType_Albedo:
		{
			for (UINT i = 0; i < m_AlbedoTextures.size(); i++) {
				if (textureID == m_AlbedoTextures[i]->GetTextureInfo().Id) {
					return m_AlbedoTextures[i];
				}
			}
			return m_DefaultAlbedoTexture;
			break;
		}
		case Texture::eTextureType::eTextureType_Normal:
		{
			for (UINT i = 0; i < m_NormalTextures.size(); i++) {

				if (textureID == m_NormalTextures[i]->GetTextureInfo().Id) {
					return m_NormalTextures[i];
				}
			}
			return m_DefaultNormalTexture;
			break;
		}
		case Texture::eTextureType::eTextureType_Roughness:
		{
			for (UINT i = 0; i < m_RoughnessTextures.size(); i++) {

				if (textureID == m_RoughnessTextures[i]->GetTextureInfo().Id) {
					return m_RoughnessTextures[i];
				}
			}
			return m_DefaultRoughnessTexture;
			break;
		}
		case Texture::eTextureType::eTextureType_Metallic:
		{
			for (UINT i = 0; i < m_MetallicTextures.size(); i++) {

				if (textureID == m_MetallicTextures[i]->GetTextureInfo().Id) {
					return m_MetallicTextures[i];
				}
			}
			return m_DefaultMetallicTexture;
			break;
		}
		case Texture::eTextureType::eTextureType_AmbientOcclusion:
		{
			for (UINT i = 0; i < m_AOTextures.size(); i++) {

				if (textureID == m_AOTextures[i]->GetTextureInfo().Id) {
					return m_AOTextures[i];
				}
			}
			return m_DefaultAOTexture;
			break;
		}
		case Texture::eTextureType::eTextureType_Opacity:
		{
			for (UINT i = 0; i < m_OpacityTextures.size(); i++) {

				if (textureID == m_OpacityTextures[i]->GetTextureInfo().Id) {
					return m_OpacityTextures[i];
				}
			}
			break;
		}
		case Texture::eTextureType::eTextureType_Translucency:
		{
			for (UINT i = 0; i < m_TranslucencyTextures.size(); i++) {

				if (textureID == m_TranslucencyTextures[i]->GetTextureInfo().Id) {
					return m_TranslucencyTextures[i];
				}
			}
			break;
		}
		default:
		{
			IE_CORE_WARN("Failed to get texture handle for texture with ID: {0}", textureID);
			break;
		}
		}

		return nullptr;
	}

	bool TextureManager::LoadDefaultTextures()
	{
		std::wstring ExeDirectory = FileSystem::GetExecutbleDirectoryW();
		ExeDirectory += L"../Default_Assets/";
		
		// Albedo
		IE_TEXTURE_INFO AlbedoTexInfo = {};
		AlbedoTexInfo.Id = -1;
		AlbedoTexInfo.GenerateMipMaps = true;
		AlbedoTexInfo.DisplayName = "Default_Albedo";
		AlbedoTexInfo.Type = Texture::eTextureType::eTextureType_Albedo;
		AlbedoTexInfo.Filepath = ExeDirectory + L"Default_Albedo.png";
		// Normal
		IE_TEXTURE_INFO NormalTexInfo = {};
		NormalTexInfo.Id = -2;
		NormalTexInfo.GenerateMipMaps = true;
		NormalTexInfo.DisplayName = "Default_Normal";
		NormalTexInfo.Type = Texture::eTextureType::eTextureType_Normal;
		NormalTexInfo.Filepath = ExeDirectory + L"Default_Normal.png";
		// Metallic
		IE_TEXTURE_INFO MetallicTexInfo = {};
		MetallicTexInfo.Id = -3;
		MetallicTexInfo.GenerateMipMaps = true;
		MetallicTexInfo.DisplayName = "Default_Metallic";
		MetallicTexInfo.Type = Texture::eTextureType::eTextureType_Metallic;
		MetallicTexInfo.Filepath = ExeDirectory + L"Default_Metallic.png";
		// Roughness
		IE_TEXTURE_INFO RoughnessTexInfo = {};
		RoughnessTexInfo.Id = -4;
		RoughnessTexInfo.GenerateMipMaps = true;
		RoughnessTexInfo.DisplayName = "Default_Roughness";
		RoughnessTexInfo.Type = Texture::eTextureType::eTextureType_Roughness;
		RoughnessTexInfo.Filepath = ExeDirectory + L"Default_RoughAO.png";
		// AO
		IE_TEXTURE_INFO AOTexInfo = {};
		AOTexInfo.Id = -5;
		AOTexInfo.GenerateMipMaps = true;
		AOTexInfo.DisplayName = "Default_AO";
		AOTexInfo.Type = Texture::eTextureType::eTextureType_AmbientOcclusion;
		AOTexInfo.Filepath = ExeDirectory + L"Default_RoughAO.png";

		switch (Renderer::GetAPI())
		{
#if defined IE_PLATFORM_WINDOWS
		case Renderer::eTargetRenderAPI::D3D_11:
		{
			m_DefaultAlbedoTexture = make_shared<ieD3D11Texture>(AlbedoTexInfo);
			m_DefaultNormalTexture = make_shared<ieD3D11Texture>(NormalTexInfo);
			m_DefaultMetallicTexture = make_shared<ieD3D11Texture>(MetallicTexInfo);
			m_DefaultRoughnessTexture = make_shared<ieD3D11Texture>(RoughnessTexInfo);
			m_DefaultAOTexture = make_shared<ieD3D11Texture>(AOTexInfo);
			break;
		}
		case Renderer::eTargetRenderAPI::D3D_12:
		{
			Direct3D12Context* graphicsContext = reinterpret_cast<Direct3D12Context*>(&Renderer::Get());
			CDescriptorHeapWrapper& cbvSrvHeapStart = graphicsContext->GetCBVSRVDescriptorHeap();

			m_DefaultAlbedoTexture = make_shared<ieD3D12Texture>(AlbedoTexInfo, cbvSrvHeapStart);
			m_DefaultNormalTexture = make_shared<ieD3D12Texture>(NormalTexInfo, cbvSrvHeapStart);
			m_DefaultMetallicTexture = make_shared<ieD3D12Texture>(MetallicTexInfo, cbvSrvHeapStart);
			m_DefaultRoughnessTexture = make_shared<ieD3D12Texture>(RoughnessTexInfo, cbvSrvHeapStart);
			m_DefaultAOTexture = make_shared<ieD3D12Texture>(AOTexInfo, cbvSrvHeapStart);
			break;
		}
#endif
		default:
		{
			IE_CORE_ERROR("Failed to load default textures for api: {0}", Renderer::GetAPI());
			break;
		}
		}
		return true;
	}

	static std::mutex s_AlbedoMutex;
	static std::mutex s_NormalMutex;
	static std::mutex s_MetallicMutex;
	static std::mutex s_RoughnessMutex;
	static std::mutex s_AOMutex;
	void TextureManager::RegisterTextureByType(const Texture::IE_TEXTURE_INFO TexInfo)
	{

		switch (Renderer::GetAPI())
		{
			case Renderer::eTargetRenderAPI::D3D_11:
			{
				switch (TexInfo.Type) {
				case Texture::eTextureType::eTextureType_Albedo:
				{
					std::lock_guard<std::mutex> Lock(s_AlbedoMutex);
					m_AlbedoTextures.push_back(std::move(make_shared<ieD3D11Texture>(TexInfo)));
					break;
				}
				case Texture::eTextureType::eTextureType_Normal:
				{
					std::lock_guard<std::mutex> Lock(s_NormalMutex);
					m_NormalTextures.push_back(std::move(make_shared<ieD3D11Texture>(TexInfo)));
					break;
				}
				case Texture::eTextureType::eTextureType_Roughness:
				{
					std::lock_guard<std::mutex> Lock(s_RoughnessMutex);
					m_RoughnessTextures.push_back(std::move(make_shared<ieD3D11Texture>(TexInfo)));
					break;
				}
				case Texture::eTextureType::eTextureType_Metallic:
				{
					std::lock_guard<std::mutex> Lock(s_MetallicMutex);
					m_MetallicTextures.push_back(std::move(make_shared<ieD3D11Texture>(TexInfo)));
					break;
				}
				case Texture::eTextureType::eTextureType_AmbientOcclusion:
				{
					std::lock_guard<std::mutex> Lock(s_AOMutex);
					m_AOTextures.push_back(std::move(make_shared<ieD3D11Texture>(TexInfo)));
					break;
				}
				case Texture::eTextureType::eTextureType_Opacity:
				{
					m_OpacityTextures.push_back(std::move(make_shared<ieD3D11Texture>(TexInfo)));
					break;
				}
				case Texture::eTextureType::eTextureType_Translucency:
				{
					m_TranslucencyTextures.push_back(std::move(make_shared<ieD3D11Texture>(TexInfo)));
					break;
				}
				default:
				{
					IE_CORE_WARN("Failed to identify texture to create with name of {0} - ID({1})", TexInfo.DisplayName, TexInfo.Id);
					break;
				}
				}
				break;
			}
			case Renderer::eTargetRenderAPI::D3D_12:
			{
				Direct3D12Context* GraphicsContext = reinterpret_cast<Direct3D12Context*>(&Renderer::Get());
				CDescriptorHeapWrapper& cbvSrvHeapStart = GraphicsContext->GetCBVSRVDescriptorHeap();

				switch (TexInfo.Type) {
				case Texture::eTextureType::eTextureType_Albedo:
				{
					std::lock_guard<std::mutex> Lock(s_AlbedoMutex);
					m_AlbedoTextures.push_back(std::move(make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart)));
					break;
				}
				case Texture::eTextureType::eTextureType_Normal:
				{
					std::lock_guard<std::mutex> Lock(s_NormalMutex);
					m_NormalTextures.push_back(std::move(make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart)));
					break;
				}
				case Texture::eTextureType::eTextureType_Roughness:
				{
					std::lock_guard<std::mutex> Lock(s_RoughnessMutex);
					m_RoughnessTextures.push_back(std::move(make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart)));
					break;
				}
				case Texture::eTextureType::eTextureType_Metallic:
				{
					std::lock_guard<std::mutex> Lock(s_MetallicMutex);
					m_MetallicTextures.push_back(std::move(make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart)));
					break;
				}
				case Texture::eTextureType::eTextureType_AmbientOcclusion:
				{
					std::lock_guard<std::mutex> Lock(s_AOMutex);
					m_AOTextures.push_back(std::move(make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart)));
					break;
				}
				case Texture::eTextureType::eTextureType_Opacity:
				{
					m_OpacityTextures.push_back(std::move(make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart)));
					break;
				}
				case Texture::eTextureType::eTextureType_Translucency:
				{
					m_TranslucencyTextures.push_back(std::move(make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart)));
					break;
				}
				default:
				{
					IE_CORE_WARN("Failed to identify texture to create with name of {0} - ID({1})", TexInfo.DisplayName, TexInfo.Id);
					break;
				}
				}
				break;
			}
			default:
			{
				IE_CORE_ERROR("Failed to determine graphics api to initialize texture. The renderer may not have been initialized yet.");
				break;
			}
		}


	}
}
