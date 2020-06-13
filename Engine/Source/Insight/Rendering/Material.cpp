#include <ie_pch.h>

#include "Material.h"

#include "Insight/Utilities/String_Helper.h"
#include "Platform/DirectX12/Direct3D12_Context.h"
#include "Insight/Systems/File_System.h"

#include "imgui.h"

namespace Insight {



	Material::Material()
	{
	}

	//Material::Material(Material&& material) noexcept
	//{
	//	//m_AlbedoMap = material.m_AlbedoMap;
	//	//m_NormalMap = material.m_NormalMap;
	//}

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

		const rapidjson::Value& jsonUVOffset = jsonMaterial["uvOffset"];
		const rapidjson::Value& jsonTilingOffset = jsonMaterial["Tiling"];
		const rapidjson::Value& jsonColorOverride = jsonMaterial["Color_Override"];
		json::get_string(jsonMaterial, "AlbedoMap", albedoTexPath);
		json::get_string(jsonMaterial, "NormalMap", normalTexPath);
		json::get_string(jsonMaterial, "MetallicMap", metallicTexPath);
		json::get_string(jsonMaterial, "RoughnessMap", roughnessTexPath);
		json::get_string(jsonMaterial, "AOMap", aoTexPath);

		json::get_float(jsonUVOffset[0], "x", m_ShaderCB.uvOffset.x);
		json::get_float(jsonUVOffset[0], "y", m_ShaderCB.uvOffset.y);

		json::get_float(jsonTilingOffset[0], "x", m_ShaderCB.tiling.x);
		json::get_float(jsonTilingOffset[0], "y", m_ShaderCB.tiling.y);

		json::get_float(jsonColorOverride[0], "r", m_ShaderCB.diffuseAdditive.x);
		json::get_float(jsonColorOverride[0], "g", m_ShaderCB.diffuseAdditive.y);
		json::get_float(jsonColorOverride[0], "b", m_ShaderCB.diffuseAdditive.z);

		json::get_float(jsonMaterial, "Metallic_Override", m_ShaderCB.metallicAdditive);
		json::get_float(jsonMaterial, "Roughness_Override", m_ShaderCB.roughnessAdditive);

		CDescriptorHeapWrapper& cbvsrvHeap = Direct3D12Context::Get().GetCBVSRVDescriptorHeap();

		Texture::IE_TEXTURE_INFO albedoMap;
		albedoMap.type = Texture::eTextureType::ALBEDO;
		albedoMap.filepath = StringHelper::StringToWide(FileSystem::Get().GetRelativeAssetDirectoryPath(albedoTexPath));
		m_AlbedoMap.Init(albedoMap, cbvsrvHeap);

		Texture::IE_TEXTURE_INFO normalMap;
		normalMap.filepath = StringHelper::StringToWide(FileSystem::Get().GetRelativeAssetDirectoryPath(normalTexPath));
		normalMap.type = Texture::eTextureType::NORMAL;
		m_NormalMap.Init(normalMap, cbvsrvHeap);

		Texture::IE_TEXTURE_INFO roughnessMapInfo;
		roughnessMapInfo.filepath = StringHelper::StringToWide(FileSystem::Get().GetRelativeAssetDirectoryPath(roughnessTexPath));
		roughnessMapInfo.type = Texture::eTextureType::ROUGHNESS;
		m_RoughnessMap.Init(roughnessMapInfo, cbvsrvHeap);

		Texture::IE_TEXTURE_INFO metallicMapInfo;
		metallicMapInfo.filepath = StringHelper::StringToWide(FileSystem::Get().GetRelativeAssetDirectoryPath(metallicTexPath));
		metallicMapInfo.type = Texture::eTextureType::METALLIC;
		m_MetallicMap.Init(metallicMapInfo, cbvsrvHeap);

		Texture::IE_TEXTURE_INFO aoMapInfo;
		aoMapInfo.filepath = StringHelper::StringToWide(FileSystem::Get().GetRelativeAssetDirectoryPath(aoTexPath));
		aoMapInfo.type = Texture::eTextureType::AO;
		m_AOMap.Init(aoMapInfo, cbvsrvHeap);

		return true;
	}

	void Material::OnImGuiRender()
	{
		if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Textures"); 

			ImGui::Text("Albedo:"); ImGui::SameLine();
			ImGui::Text(m_AlbedoMap.GetDisplayName().c_str());

			ImGui::Text("Normal:"); ImGui::SameLine();
			ImGui::Text(m_NormalMap.GetDisplayName().c_str());

			ImGui::Text("Roughness:"); ImGui::SameLine();
			ImGui::Text(m_RoughnessMap.GetDisplayName().c_str());

			ImGui::Text("Metallic:"); ImGui::SameLine();
			ImGui::Text(m_MetallicMap.GetDisplayName().c_str());

			ImGui::Text("AO:"); ImGui::SameLine();
			ImGui::Text(m_AOMap.GetDisplayName().c_str());

			ImGui::Spacing();

			ImGui::Text("PBR Offsets");
			ImGuiColorEditFlags colorWheelFlags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_PickerHueWheel;
			// Imgui will edit the color values in a normalized 0 to 1 space. 
			// In the shaders we transform the color values back into 0 to 255 space.
			ImGui::ColorEdit3("Diffuse Additive: ", &m_ShaderCB.diffuseAdditive.x, colorWheelFlags);
			ImGui::DragFloat("Metallic Addative:", &m_ShaderCB.metallicAdditive, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Roughness Addative:", &m_ShaderCB.roughnessAdditive, 0.01f, 0.0f, 1.0f);

			ImGui::Text("UVs");
			ImGui::DragFloat2("Tiling:", &m_ShaderCB.tiling.x, 0.01f, 0.0f, 10.0f);
			ImGui::DragFloat2("Offset:", &m_ShaderCB.uvOffset.x, 0.01f, 0.0f, 10.0f);
		}

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