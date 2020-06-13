#include <ie_pch.h>

#include "Texture_Manager.h"
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
		for (UINT i = 0; i < m_AlbedoTextures.size(); i++) {
			delete m_AlbedoTextures[i];
		}

		for (UINT i = 0; i < m_NormalTextures.size(); i++) {
			delete m_NormalTextures[i];
		}

		for (UINT i = 0; i < m_MetallicTextures.size(); i++) {
			delete m_MetallicTextures[i];
		}

		for (UINT i = 0; i < m_RoughnessTextures.size(); i++) {
			delete m_RoughnessTextures[i];
		}

		for (UINT i = 0; i < m_AOTextures.size(); i++) {
			delete m_AOTextures[i];
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
			int type;
			bool genMipMaps;
			json::get_string(jsonTextures[i], "Name", name);
			json::get_string(jsonTextures[i], "Filepath", filepath);
			json::get_int(jsonTextures[i], "Type", type);
			json::get_bool(jsonTextures[i], "GenerateMipMaps", genMipMaps);

			Texture::IE_TEXTURE_INFO texInfo = {};
			texInfo.displayName = name;
			texInfo.filepath = StringHelper::StringToWide(filepath);
			texInfo.generateMipMaps = genMipMaps;
			texInfo.type = (Texture::eTextureType)type;

			LoadTextureByType(texInfo);
		}

		return true;
	}
	
	void TextureManager::LoadTextureByType(const Texture::IE_TEXTURE_INFO& texInfo)
	{
		Direct3D12Context& graphicsContext = Direct3D12Context::Get();
		CDescriptorHeapWrapper& cbvSrvHeapStart = graphicsContext.GetCBVSRVDescriptorHeap();

		switch (texInfo.type) {
		case Texture::eTextureType::ALBEDO:
		{
			m_AlbedoTextures.push_back(new Texture(texInfo, cbvSrvHeapStart));
			break;
		}
		case Texture::eTextureType::NORMAL:
		{
			m_NormalTextures.push_back(new Texture(texInfo, cbvSrvHeapStart));
			break;
		}
		case Texture::eTextureType::ROUGHNESS:
		{
			m_RoughnessTextures.push_back(new Texture(texInfo, cbvSrvHeapStart));
			break;
		}
		case Texture::eTextureType::METALLIC:
		{
			m_MetallicTextures.push_back(new Texture(texInfo, cbvSrvHeapStart));
			break;
		}
		case Texture::eTextureType::AO:
		{
			m_AOTextures.push_back(new Texture(texInfo, cbvSrvHeapStart));
			break;
		}
		default:
		{
			IE_CORE_WARN("Failed to create texture {0}", texInfo.displayName);
			break;
		}
		}
	}
}
