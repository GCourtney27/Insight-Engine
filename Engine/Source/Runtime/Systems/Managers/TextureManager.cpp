#include <Engine_pch.h>

#include "TextureManager.h"
#include "Runtime/Utilities/StringHelper.h"
#include "Runtime/Rendering/Renderer.h"

#include "Platform/DirectX12/Direct3D12Context.h"
#include "Platform/DirectX12/Wrappers/D3D12Texture.h"
#include "Platform/DirectX11/Wrappers/D3D11Texture.h"

namespace Insight {



	TextureManager::TextureManager()
		: m_HighestTextureId(0u)
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
		m_AlbedoTextureMap.clear();
		m_NormalTextureMap.clear();
		m_MetallicTextureMap.clear();
		m_RoughnessTextureMap.clear();
		m_AOTextureMap.clear();
		m_OpacityTextureMap.clear();
		m_TranslucencyTextureMap.clear();


	}

	bool TextureManager::Init()
	{
		LoadDefaultTextures();
		return true;
	}

	bool TextureManager::PostInit()
	{
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
			TexInfo.Id = ID;
			TexInfo.Filepath = FileSystem::GetRelativeContentDirectoryW(StringHelper::StringToWide(Filepath));
			TexInfo.GenerateMipMaps = GenMipMaps;
			TexInfo.Type = (Texture::eTextureType)Type;

			m_TextureLoadFutures.push_back(std::async(std::launch::async, &TextureManager::RegisterTextureByType, this, TexInfo));
			//RegisterTextureByType(TexInfo);

			m_HighestTextureId = ((int)m_HighestTextureId < ID) ? ID : m_HighestTextureId;
		}

		return true;
	}

	StrongTexturePtr TextureManager::GetTextureByID(Texture::ID TextureID, Texture::eTextureType TextureType)
	{
		switch (TextureType) 
		{
		case Texture::eTextureType::eTextureType_Albedo:
		{
			auto Iter = m_AlbedoTextureMap.find(TextureID);
			if (Iter != m_AlbedoTextureMap.end()) {
				return (*Iter).second;
			}
			else {
				return m_DefaultAlbedoTexture;
			}
		}
		case Texture::eTextureType::eTextureType_Normal:
		{
			auto Iter = m_NormalTextureMap.find(TextureID);
			if (Iter != m_NormalTextureMap.end()) {
				return (*Iter).second;
			}
			else {
				return m_DefaultNormalTexture;
			}
		}
		case Texture::eTextureType::eTextureType_Roughness:
		{
			auto Iter = m_RoughnessTextureMap.find(TextureID);
			if (Iter != m_RoughnessTextureMap.end()) {
				return (*Iter).second;
			}
			else {
				return m_DefaultRoughnessTexture;
			}
		}
		case Texture::eTextureType::eTextureType_Metallic:
		{
			auto Iter = m_MetallicTextureMap.find(TextureID);
			if (Iter != m_MetallicTextureMap.end()) {
				return (*Iter).second;
			}
			else {
				return m_DefaultMetallicTexture;
			}
		}
		case Texture::eTextureType::eTextureType_AmbientOcclusion:
		{
			auto Iter = m_AOTextureMap.find(TextureID);
			if (Iter != m_AOTextureMap.end()) {
				return (*Iter).second;
			}
			else {
				return m_DefaultAOTexture;
			}
		}
		case Texture::eTextureType::eTextureType_Opacity:
		{
			auto Iter = m_OpacityTextureMap.find(TextureID);
			if (Iter != m_OpacityTextureMap.end()) {
				return (*Iter).second;
			}
			else {
				return m_DefaultAOTexture;
			}
		}
		case Texture::eTextureType::eTextureType_Translucency:
		{
			auto Iter = m_TranslucencyTextureMap.find(TextureID);
			if (Iter != m_TranslucencyTextureMap.end()) {
				return (*Iter).second;
			}
			else {
				return m_DefaultAOTexture;
			}
		}
		default:
		{
			IE_DEBUG_LOG(LogSeverity::Warning, "Failed to get texture handle for texture with ID: {0}", TextureID);
			break;
		}
		}

		return nullptr;
	}

	void TextureManager::RegisterTextureLoadCallback(Texture::ID AwaitingTextureId, StrongTexturePtr* AwaitingTexture)
	{
		auto Iter = m_AwaitingLoadTextures.find(AwaitingTextureId);
		if (Iter != m_AwaitingLoadTextures.end())
		{
			// Add the texture to the queue to be initialized once the asset is created.
			(*Iter).second.push_back(AwaitingTexture);
		}
		else
		{
			// If the element does not exist, create a new list and initialize it with the texture.
			m_AwaitingLoadTextures.insert({ AwaitingTextureId, { AwaitingTexture } });
		}
	}

	bool TextureManager::LoadDefaultTextures()
	{
		const wchar_t* ExeDir = FileSystem::GetWorkingDirectoryW();
		std::wstring DefaultAssetDirectory = FileSystem::GetRelativeContentDirectoryW(L"Engine/Textures/Default_Object/");
		
		// Albedo
		IE_TEXTURE_INFO AlbedoTexInfo = {};
		AlbedoTexInfo.Id = DEFAULT_ALBEDO_TEXTURE_ID;
		AlbedoTexInfo.GenerateMipMaps = true;
		AlbedoTexInfo.Type = Texture::eTextureType::eTextureType_Albedo;
		AlbedoTexInfo.Filepath = DefaultAssetDirectory + L"Default_Albedo.png";
		// Normal
		IE_TEXTURE_INFO NormalTexInfo = {};
		NormalTexInfo.Id = DEFAULT_NORMAL_TEXTURE_ID;
		NormalTexInfo.GenerateMipMaps = true;
		NormalTexInfo.Type = Texture::eTextureType::eTextureType_Normal;
		NormalTexInfo.Filepath = DefaultAssetDirectory + L"Default_Normal.png";
		// Metallic
		IE_TEXTURE_INFO MetallicTexInfo = {};
		MetallicTexInfo.Id = DEFAULT_METALLIC_TEXTURE_ID;
		MetallicTexInfo.GenerateMipMaps = true;
		MetallicTexInfo.Type = Texture::eTextureType::eTextureType_Metallic;
		MetallicTexInfo.Filepath = DefaultAssetDirectory + L"Default_Metallic.png";
		// Roughness
		IE_TEXTURE_INFO RoughnessTexInfo = {};
		RoughnessTexInfo.Id = DEFAULT_ROUGHNESS_TEXTURE_ID;
		RoughnessTexInfo.GenerateMipMaps = true;
		RoughnessTexInfo.Type = Texture::eTextureType::eTextureType_Roughness;
		RoughnessTexInfo.Filepath = DefaultAssetDirectory + L"Default_RoughAO.png";
		// AO
		IE_TEXTURE_INFO AOTexInfo = {};
		AOTexInfo.Id = DEFAULT_AO_TEXTURE_ID;
		AOTexInfo.GenerateMipMaps = true;
		AOTexInfo.Type = Texture::eTextureType::eTextureType_AmbientOcclusion;
		AOTexInfo.Filepath = DefaultAssetDirectory + L"Default_RoughAO.png";

		switch (Renderer::GetAPI())
		{
#if defined (IE_PLATFORM_WINDOWS)
		case Renderer::TargetRenderAPI::Direct3D_11:
		{
			m_DefaultAlbedoTexture = make_shared<ieD3D11Texture>(AlbedoTexInfo);
			m_DefaultNormalTexture = make_shared<ieD3D11Texture>(NormalTexInfo);
			m_DefaultMetallicTexture = make_shared<ieD3D11Texture>(MetallicTexInfo);
			m_DefaultRoughnessTexture = make_shared<ieD3D11Texture>(RoughnessTexInfo);
			m_DefaultAOTexture = make_shared<ieD3D11Texture>(AOTexInfo);
			break;
		}
		case Renderer::TargetRenderAPI::Direct3D_12:
		{
			Direct3D12Context& RenderContext = Renderer::GetAs<Direct3D12Context>();
			CDescriptorHeapWrapper& cbvSrvHeapStart = RenderContext.GetCBVSRVDescriptorHeap();

			m_DefaultAlbedoTexture = make_shared<ieD3D12Texture>(AlbedoTexInfo, cbvSrvHeapStart);
			m_DefaultNormalTexture = make_shared<ieD3D12Texture>(NormalTexInfo, cbvSrvHeapStart);
			m_DefaultMetallicTexture = make_shared<ieD3D12Texture>(MetallicTexInfo, cbvSrvHeapStart);
			m_DefaultRoughnessTexture = make_shared<ieD3D12Texture>(RoughnessTexInfo, cbvSrvHeapStart);
			m_DefaultAOTexture = make_shared<ieD3D12Texture>(AOTexInfo, cbvSrvHeapStart);
			break;
		}
#endif // IE_PLATFORM_WINDOWS
		default:
		{
			IE_DEBUG_LOG(LogSeverity::Error, "Failed to load default textures for api: {0}", Renderer::GetAPI());
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
	static std::mutex s_OpacityMutex;
	static std::mutex s_TranslucencyMutex;
	void TextureManager::RegisterTextureByType(const IE_TEXTURE_INFO TexInfo)
	{
		switch (Renderer::GetAPI())
		{
			case Renderer::TargetRenderAPI::Direct3D_11:
			{
				switch (TexInfo.Type) {
				case Texture::eTextureType::eTextureType_Albedo:
				{
					std::lock_guard<std::mutex> Lock(s_AlbedoMutex);
					m_AlbedoTextureMap.insert({ TexInfo.Id, std::move(make_shared<ieD3D11Texture>(TexInfo)) });
					break;
				}
				case Texture::eTextureType::eTextureType_Normal:
				{
					std::lock_guard<std::mutex> Lock(s_NormalMutex);
					m_NormalTextureMap.insert({ TexInfo.Id, std::move(make_shared<ieD3D11Texture>(TexInfo)) });
					break;
				}
				case Texture::eTextureType::eTextureType_Roughness:
				{
					std::lock_guard<std::mutex> Lock(s_RoughnessMutex);
					m_RoughnessTextureMap.insert({ TexInfo.Id, std::move(make_shared<ieD3D11Texture>(TexInfo)) });
					break;
				}
				case Texture::eTextureType::eTextureType_Metallic:
				{
					std::lock_guard<std::mutex> Lock(s_MetallicMutex);
					m_MetallicTextureMap.insert({ TexInfo.Id, std::move(make_shared<ieD3D11Texture>(TexInfo)) });
					break;
				}
				case Texture::eTextureType::eTextureType_AmbientOcclusion:
				{
					std::lock_guard<std::mutex> Lock(s_AOMutex);
					m_AOTextureMap.insert({ TexInfo.Id, std::move(make_shared<ieD3D11Texture>(TexInfo)) });
					break;
				}
				case Texture::eTextureType::eTextureType_Opacity:
				{
					std::lock_guard<std::mutex> Lock(s_OpacityMutex);
					m_OpacityTextureMap.insert({ TexInfo.Id, std::move(make_shared<ieD3D11Texture>(TexInfo)) });
					break;
				}
				case Texture::eTextureType::eTextureType_Translucency:
				{
					std::lock_guard<std::mutex> Lock(s_TranslucencyMutex);
					m_TranslucencyTextureMap.insert({ TexInfo.Id, std::move(make_shared<ieD3D11Texture>(TexInfo)) });
					break;
				}
				default:
				{
					IE_DEBUG_LOG(LogSeverity::Warning, "Failed to identify texture to create with ID of {0}", TexInfo.Id);
					break;
				}
				}
				break;
			}
			case Renderer::TargetRenderAPI::Direct3D_12:
			{
				Direct3D12Context& RenderContext = Renderer::GetAs<Direct3D12Context>();
				CDescriptorHeapWrapper& cbvSrvHeapStart = RenderContext.GetCBVSRVDescriptorHeap();

				switch (TexInfo.Type) {
				case Texture::eTextureType::eTextureType_Albedo:
				{
					std::lock_guard<std::mutex> Lock(s_AlbedoMutex);
					m_AlbedoTextureMap.insert({ TexInfo.Id, std::move(make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart)) });
					break;
				}
				case Texture::eTextureType::eTextureType_Normal:
				{
					std::lock_guard<std::mutex> Lock(s_NormalMutex);
					m_NormalTextureMap.insert({ TexInfo.Id, std::move(make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart)) });
					break;
				}
				case Texture::eTextureType::eTextureType_Roughness:
				{
					std::lock_guard<std::mutex> Lock(s_RoughnessMutex);
					m_RoughnessTextureMap.insert({ TexInfo.Id, std::move(make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart)) });
					break;
				}
				case Texture::eTextureType::eTextureType_Metallic:
				{
					std::lock_guard<std::mutex> Lock(s_MetallicMutex);
					m_MetallicTextureMap.insert({ TexInfo.Id, std::move(make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart)) });
					break;
				}
				case Texture::eTextureType::eTextureType_AmbientOcclusion:
				{
					std::lock_guard<std::mutex> Lock(s_AOMutex);
					m_AOTextureMap.insert({ TexInfo.Id, std::move(make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart)) });
					break;
				}
				case Texture::eTextureType::eTextureType_Opacity:
				{
					std::lock_guard<std::mutex> Lock(s_OpacityMutex);
					m_OpacityTextureMap.insert({ TexInfo.Id, std::move(make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart)) });
					break;
				}
				case Texture::eTextureType::eTextureType_Translucency:
				{
					std::lock_guard<std::mutex> Lock(s_TranslucencyMutex);
					m_TranslucencyTextureMap.insert({ TexInfo.Id, std::move(make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart)) });
					break;
				}
				default:
				{
					IE_DEBUG_LOG(LogSeverity::Warning, "Failed to identify texture to create with ID of: {0}", TexInfo.Id);
					break;
				}
				}
				break;
			}
			default:
			{
				IE_DEBUG_LOG(LogSeverity::Error, "Failed to determine graphics api to initialize texture. The renderer may not have been initialized yet.");
				break;
			}

		} // end switch(Renderer::GetAPI())
				
		// Check if the loaded texture is currently being waited upon by any materials.
		auto Iter = m_AwaitingLoadTextures.find(TexInfo.Id);
		if (Iter != m_AwaitingLoadTextures.end())
		{
			// Reasign the texture in the material. There could be multiple materials waiting for the texture.
			for (auto Tex : (*Iter).second)
			{
				*Tex = GetTextureByID(TexInfo.Id, TexInfo.Type);;
			}
			// Erase the element from the map.
			m_AwaitingLoadTextures.erase(Iter);
		}
	}
}
