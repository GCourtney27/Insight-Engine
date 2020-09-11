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
		for (auto [Id, Tex] : m_AlbedoTextureMap) {
			Tex.reset();
		}
		for (auto [Id, Tex] : m_NormalTextureMap) {
			Tex.reset();
		}
		for (auto [Id, Tex] : m_MetallicTextureMap) {
			Tex.reset();
		}
		for (auto [Id, Tex] : m_RoughnessTextureMap) {
			Tex.reset();
		}
		for (auto [Id, Tex] : m_AOTextureMap) {
			Tex.reset();
		}
		for (auto [Id, Tex] : m_OpacityTextureMap) {
			Tex.reset();
		}
		for (auto [Id, Tex] : m_TranslucencyTextureMap) {
			Tex.reset();
		}

	}

	bool TextureManager::Init()
	{
		LoadDefaultTextures();
		return true;
	}

	bool TextureManager::PostInit()
	{
		return true;

		for (auto [Key, Value] : m_AwaitingLoadTextures)
		{
			StrongTexturePtr Tex = GetTextureByID(Key, (*Value)->GetTextureInfo().Type);
			if (Tex != nullptr)
			{
				(*Value) = Tex;
				m_AwaitingLoadTextures.erase(Key);
			}
		}

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

			m_TextureLoadFutures.push_back(std::async(std::launch::async, &TextureManager::RegisterTextureByType, this, TexInfo));
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
			auto Iter = m_AlbedoTextureMap.find(textureID);
			if (Iter != m_AlbedoTextureMap.end()) {
				return (*Iter).second;
			}
			else {
				return m_DefaultAlbedoTexture;
			}
		}
		case Texture::eTextureType::eTextureType_Normal:
		{
			auto Iter = m_NormalTextureMap.find(textureID);
			if (Iter != m_NormalTextureMap.end()) {
				return (*Iter).second;
			}
			else {
				return m_DefaultNormalTexture;
			}
		}
		case Texture::eTextureType::eTextureType_Roughness:
		{
			auto Iter = m_RoughnessTextureMap.find(textureID);
			if (Iter != m_RoughnessTextureMap.end()) {
				return (*Iter).second;
			}
			else {
				return m_DefaultRoughnessTexture;
			}
		}
		case Texture::eTextureType::eTextureType_Metallic:
		{
			auto Iter = m_MetallicTextureMap.find(textureID);
			if (Iter != m_MetallicTextureMap.end()) {
				return (*Iter).second;
			}
			else {
				return m_DefaultMetallicTexture;
			}
		}
		case Texture::eTextureType::eTextureType_AmbientOcclusion:
		{
			auto Iter = m_AOTextureMap.find(textureID);
			if (Iter != m_AOTextureMap.end()) {
				return (*Iter).second;
			}
			else {
				return m_DefaultAOTexture;
			}
		}
		case Texture::eTextureType::eTextureType_Opacity:
		{
			auto Iter = m_OpacityTextureMap.find(textureID);
			if (Iter != m_OpacityTextureMap.end()) {
				return (*Iter).second;
			}
			else {
				return m_DefaultAOTexture;
			}
		}
		case Texture::eTextureType::eTextureType_Translucency:
		{
			auto Iter = m_TranslucencyTextureMap.find(textureID);
			if (Iter != m_TranslucencyTextureMap.end()) {
				return (*Iter).second;
			}
			else {
				return m_DefaultAOTexture;
			}
		}
		default:
		{
			IE_CORE_WARN("Failed to get texture handle for texture with ID: {0}", textureID);
			break;
		}
		}

		return nullptr;
	}

	void TextureManager::RegisterTextureLoadCallback(Texture::ID AwaitingTextureId, StrongTexturePtr* AwaitingTexture)
	{
		m_AwaitingLoadTextures.insert({ AwaitingTextureId, AwaitingTexture });
	}

	bool TextureManager::LoadDefaultTextures()
	{
		std::wstring ExeDirectory = FileSystem::GetExecutbleDirectoryW();
		ExeDirectory += L"../Default_Assets/";
		
		// Albedo
		IE_TEXTURE_INFO AlbedoTexInfo = {};
		AlbedoTexInfo.Id = DEFAULT_ALBEDO_TEXTURE_ID;
		AlbedoTexInfo.GenerateMipMaps = true;
		AlbedoTexInfo.DisplayName = "Default_Albedo";
		AlbedoTexInfo.Type = Texture::eTextureType::eTextureType_Albedo;
		AlbedoTexInfo.Filepath = ExeDirectory + L"Default_Albedo.png";
		// Normal
		IE_TEXTURE_INFO NormalTexInfo = {};
		NormalTexInfo.Id = DEFAULT_NORMAL_TEXTURE_ID;
		NormalTexInfo.GenerateMipMaps = true;
		NormalTexInfo.DisplayName = "Default_Normal";
		NormalTexInfo.Type = Texture::eTextureType::eTextureType_Normal;
		NormalTexInfo.Filepath = ExeDirectory + L"Default_Normal.png";
		// Metallic
		IE_TEXTURE_INFO MetallicTexInfo = {};
		MetallicTexInfo.Id = DEFAULT_METALLIC_TEXTURE_ID;
		MetallicTexInfo.GenerateMipMaps = true;
		MetallicTexInfo.DisplayName = "Default_Metallic";
		MetallicTexInfo.Type = Texture::eTextureType::eTextureType_Metallic;
		MetallicTexInfo.Filepath = ExeDirectory + L"Default_Metallic.png";
		// Roughness
		IE_TEXTURE_INFO RoughnessTexInfo = {};
		RoughnessTexInfo.Id = DEFAULT_ROUGHNESS_TEXTURE_ID;
		RoughnessTexInfo.GenerateMipMaps = true;
		RoughnessTexInfo.DisplayName = "Default_Roughness";
		RoughnessTexInfo.Type = Texture::eTextureType::eTextureType_Roughness;
		RoughnessTexInfo.Filepath = ExeDirectory + L"Default_RoughAO.png";
		// AO
		IE_TEXTURE_INFO AOTexInfo = {};
		AOTexInfo.Id = DEFAULT_AO_TEXTURE_ID;
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
	static std::mutex s_OpacityMutex;
	static std::mutex s_TranslucencyMutex;
	void TextureManager::RegisterTextureByType(const IE_TEXTURE_INFO TexInfo)
	{
		switch (Renderer::GetAPI())
		{
			case Renderer::eTargetRenderAPI::D3D_11:
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

		} // end switch(Renderer::GetAPI())

		//auto Iter = m_AwaitingLoadTextures.find({ TexInfo.Id, TexInfo.Type });
		
		//if (Iter != m_AwaitingLoadTextures.end())
		//{
		//	// Reasign the texture in the material.
		//	(*(*Iter).second) = GetTextureByID(TexInfo.Id, TexInfo.Type);
		//	// Erase the pointer from the map.
		//	m_AwaitingLoadTextures.erase({ TexInfo.Id, TexInfo.Type });
		//}
		
		// Check if the loaded texture is currently being waited upon by any materials.
		auto Iter = m_AwaitingLoadTextures.find(TexInfo.Id);

		while (Iter != m_AwaitingLoadTextures.end())
		{
			// Reasign the texture in the material.
			(*(*Iter).second) = GetTextureByID(TexInfo.Id, TexInfo.Type);
			// Erase the pointer from the map.
			m_AwaitingLoadTextures.erase(Iter);
			Iter = m_AwaitingLoadTextures.find(TexInfo.Id);
		}


	}
}
