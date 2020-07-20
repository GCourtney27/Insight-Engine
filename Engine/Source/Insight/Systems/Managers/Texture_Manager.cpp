#include <ie_pch.h>

#include "Texture_Manager.h"
#include "Insight/Systems/File_System.h"
#include "Insight/Utilities/String_Helper.h"
#include "Platform/Windows/DirectX_12/Direct3D12_Context.h"

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
	}

	bool TextureManager::Init()
	{
		LoadDefaultTextures();
		return true;
	}

	bool TextureManager::LoadResourcesFromJson(const rapidjson::Value& JsonTextures)
	{
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
			
			RegisterTextureByType(TexInfo);

			m_HighestTextureId = ((int)m_HighestTextureId < ID) ? ID : m_HighestTextureId;
		}

		return true;
	}

	StrongTexturePtr TextureManager::GetTextureByID(Texture::ID textureID, Texture::eTextureType textreType)
	{
		switch (textreType) {
		case Texture::eTextureType::ALBEDO:
		{
			for (UINT i = 0; i < m_AlbedoTextures.size(); i++) {
				if (textureID == m_AlbedoTextures[i]->GetTextureInfo().Id) {
					return m_AlbedoTextures[i];
				}
			}
			break;
		}
		case Texture::eTextureType::NORMAL:
		{
			for (UINT i = 0; i < m_NormalTextures.size(); i++) {

				if (textureID == m_NormalTextures[i]->GetTextureInfo().Id) {
					return m_NormalTextures[i];
				}
			}
			break;
		}
		case Texture::eTextureType::ROUGHNESS:
		{
			for (UINT i = 0; i < m_RoughnessTextures.size(); i++) {

				if (textureID == m_RoughnessTextures[i]->GetTextureInfo().Id) {
					return m_RoughnessTextures[i];
				}
			}
			break;
		}
		case Texture::eTextureType::METALLIC:
		{
			for (UINT i = 0; i < m_MetallicTextures.size(); i++) {

				if (textureID == m_MetallicTextures[i]->GetTextureInfo().Id) {
					return m_MetallicTextures[i];
				}
			}
			break;
		}
		case Texture::eTextureType::AO:
		{
			for (UINT i = 0; i < m_AOTextures.size(); i++) {

				if (textureID == m_AOTextures[i]->GetTextureInfo().Id) {
					return m_AOTextures[i];
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
		Direct3D12Context& graphicsContext = Direct3D12Context::Get();
		CDescriptorHeapWrapper& cbvSrvHeapStart = graphicsContext.GetCBVSRVDescriptorHeap();

		Texture::IE_TEXTURE_INFO TexInfo = {};
		TexInfo.Id = -1;
		TexInfo.GenerateMipMaps = true;

		TexInfo.DisplayName = "Default_Albedo";
		TexInfo.Type = Texture::eTextureType::ALBEDO;
		TexInfo.Filepath = StringHelper::StringToWide("Assets/Textures/Default_Object/Default_Albedo.png");
		m_DefaultAlbedoTexture = make_shared<Texture>(TexInfo, cbvSrvHeapStart);

		TexInfo.DisplayName = "Default_Normal";
		TexInfo.Type = Texture::eTextureType::NORMAL;
		TexInfo.Filepath = StringHelper::StringToWide("Assets/Textures/Default_Object/Default_Normal.png");
		m_DefaultNormalTexture = make_shared<Texture>(TexInfo, cbvSrvHeapStart);

		TexInfo.DisplayName = "Default_Metallic";
		TexInfo.Type = Texture::eTextureType::METALLIC;
		TexInfo.Filepath = StringHelper::StringToWide("Assets/Textures/Default_Object/Default_Metallic.png");
		m_DefaultMetallicTexture = make_shared<Texture>(TexInfo, cbvSrvHeapStart);

		TexInfo.DisplayName = "Default_Roughness";
		TexInfo.Type = Texture::eTextureType::ROUGHNESS;
		TexInfo.Filepath = StringHelper::StringToWide("Assets/Textures/Default_Object/Default_RoughAO.png");
		m_DefaultRoughnessTexture = make_shared<Texture>(TexInfo, cbvSrvHeapStart);
		
		TexInfo.DisplayName = "Default_AO";
		TexInfo.Type = Texture::eTextureType::AO;
		TexInfo.Filepath = StringHelper::StringToWide("Assets/Textures/Default_Object/Default_RoughAO.png");
		m_DefaultAOTexture = make_shared<Texture>(TexInfo, cbvSrvHeapStart);

		return true;
	}

	void TextureManager::RegisterTextureByType(const Texture::IE_TEXTURE_INFO& texInfo)
	{
		Direct3D12Context& graphicsContext = Direct3D12Context::Get();
		CDescriptorHeapWrapper& cbvSrvHeapStart = graphicsContext.GetCBVSRVDescriptorHeap();

		switch (texInfo.Type) {
		case Texture::eTextureType::ALBEDO:
		{
			m_AlbedoTextures.push_back(make_shared<Texture>(texInfo, cbvSrvHeapStart));
			break;
		}
		case Texture::eTextureType::NORMAL:
		{
			m_NormalTextures.push_back(make_shared<Texture>(texInfo, cbvSrvHeapStart));
			break;
		}
		case Texture::eTextureType::ROUGHNESS:
		{
			m_RoughnessTextures.push_back(make_shared<Texture>(texInfo, cbvSrvHeapStart));
			break;
		}
		case Texture::eTextureType::METALLIC:
		{
			m_MetallicTextures.push_back(make_shared<Texture>(texInfo, cbvSrvHeapStart));
			break;
		}
		case Texture::eTextureType::AO:
		{
			m_AOTextures.push_back(make_shared<Texture>(texInfo, cbvSrvHeapStart));
			break;
		}
		default:
		{
			IE_CORE_WARN("Failed to identify texture to create with name of {0} - ID({1})", texInfo.DisplayName, texInfo.Id);
			break;
		}
		}
	}
}
