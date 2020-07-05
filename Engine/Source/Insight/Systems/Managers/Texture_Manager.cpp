#include <ie_pch.h>

#include "Texture_Manager.h"
#include "Insight/Systems/File_System.h"
#include "Insight/Utilities/String_Helper.h"
#include "Platform/DirectX12/Direct3D12_Context.h"

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
		return true;
	}

	bool TextureManager::LoadResourcesFromJson(const rapidjson::Value& jsonTextures)
	{
		for (rapidjson::SizeType i = 0; i < jsonTextures.Size(); i++) {
			std::string name, filepath;
			int type, id;
			bool genMipMaps;
			json::get_int(jsonTextures[i], "ID", id);
			json::get_int(jsonTextures[i], "Type", type);
			json::get_string(jsonTextures[i], "Name", name);
			json::get_string(jsonTextures[i], "Filepath", filepath);
			json::get_bool(jsonTextures[i], "GenerateMipMaps", genMipMaps);

			Texture::IE_TEXTURE_INFO texInfo = {};
			texInfo.DisplayName = name;
			texInfo.Id = id;
			texInfo.Filepath = StringHelper::StringToWide(FileSystem::Get().GetRelativeAssetDirectoryPath(filepath));
			texInfo.GenerateMipMaps = genMipMaps;
			texInfo.Type = (Texture::eTextureType)type;
			
			RegisterTextureByType(texInfo);

			m_HighestTextureId = ((int)m_HighestTextureId < id) ? id : m_HighestTextureId;
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
