#include <ie_pch.h>

#include "Material.h"

#include "Insight/Utilities/String_Helper.h"
#include "Platform/DirectX12/Direct3D12_Context.h"
#include "Insight/Systems/File_System.h"

namespace Insight {



	Material::Material()
	{
	}

	Material::~Material()
	{
	}

	bool Material::LoadFromJson(const rapidjson::Value& jsonMaterial)
	{
		std::string albedoTexPath;
		std::string normalTexPath;
		std::string metallicTexPath;
		std::string roughnessTexPath;
		std::string aoTexPath;

		json::get_string(jsonMaterial, "AlbedoMap", albedoTexPath);
		json::get_string(jsonMaterial, "NormalMap", normalTexPath);
		json::get_string(jsonMaterial, "MetallicMap", metallicTexPath);
		json::get_string(jsonMaterial, "RoughnessMap", roughnessTexPath);
		json::get_string(jsonMaterial, "AOMap", aoTexPath);

		//const rapidjson::Value& uvParams = jsonMaterial["uvOffset"];
		//json::get_float(jsonMaterial, uvParams[0]["x"], )

		CDescriptorHeapWrapper& cbvsrvHeap = Direct3D12Context::Get().GetCBVSRVDescriptorHeap();

		std::wstring texRelPathAlbedoW = StringHelper::StringToWide(FileSystem::Get().GetRelativeAssetDirectoryPath(albedoTexPath));
		m_AlbedoMap.Init(texRelPathAlbedoW, Texture::eTextureType::ALBEDO, cbvsrvHeap);

		std::wstring texRelPathNormalW = StringHelper::StringToWide(FileSystem::Get().GetRelativeAssetDirectoryPath(normalTexPath));
		m_NormalMap.Init(texRelPathNormalW, Texture::eTextureType::NORMAL, cbvsrvHeap);

		std::wstring texRelPathRoughnessW = StringHelper::StringToWide(FileSystem::Get().GetRelativeAssetDirectoryPath(roughnessTexPath));
		m_RoughnessMap.Init(texRelPathRoughnessW, Texture::eTextureType::ROUGHNESS, cbvsrvHeap);

		std::wstring texRelPathMetallicW = StringHelper::StringToWide(FileSystem::Get().GetRelativeAssetDirectoryPath(metallicTexPath));
		m_MetallicMap.Init(texRelPathMetallicW, Texture::eTextureType::METALLIC, cbvsrvHeap);

		std::wstring texRelPathAOW = StringHelper::StringToWide(FileSystem::Get().GetRelativeAssetDirectoryPath(aoTexPath));
		m_AOMap.Init(texRelPathAOW, Texture::eTextureType::AO, cbvsrvHeap);

		return true;
	}

	void Material::OnImGuiRender()
	{
	}

	void Material::BindResources()
	{
		m_AlbedoMap.Bind();
		m_NormalMap.Bind();
		m_MetallicMap.Bind();
		m_RoughnessMap.Bind();
		m_AOMap.Bind();
	}

}