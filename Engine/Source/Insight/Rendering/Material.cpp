#include <ie_pch.h>

#include "Material.h"

#include "Insight/Utilities/String_Helper.h"
#include "Platform/DirectX12/Direct3D12_Context.h"
#include "Insight/Systems/File_System.h"
#include "Insight/Systems/Managers/Resource_Manager.h"

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
		int albedoTexID;
		int normalTexID;
		int metallicTexID;
		int roughnessTexID;
		int aoTexID;
		// TODO get texture handle by id from textre manager
		const rapidjson::Value& jsonUVOffset = jsonMaterial["uvOffset"];
		const rapidjson::Value& jsonTilingOffset = jsonMaterial["Tiling"];
		const rapidjson::Value& jsonColorOverride = jsonMaterial["Color_Override"];
		json::get_int(jsonMaterial, "AlbedoMapID", albedoTexID);
		json::get_int(jsonMaterial, "NormalMapID", normalTexID);
		json::get_int(jsonMaterial, "MetallicMapID", metallicTexID);
		json::get_int(jsonMaterial, "RoughnessMapID", roughnessTexID);
		json::get_int(jsonMaterial, "AOMapID", aoTexID);

		json::get_float(jsonUVOffset[0], "x", m_ShaderCB.uvOffset.x);
		json::get_float(jsonUVOffset[0], "y", m_ShaderCB.uvOffset.y);

		json::get_float(jsonTilingOffset[0], "u", m_ShaderCB.tiling.x);
		json::get_float(jsonTilingOffset[0], "v", m_ShaderCB.tiling.y);

		json::get_float(jsonColorOverride[0], "r", m_ShaderCB.diffuseAdditive.x);
		json::get_float(jsonColorOverride[0], "g", m_ShaderCB.diffuseAdditive.y);
		json::get_float(jsonColorOverride[0], "b", m_ShaderCB.diffuseAdditive.z);

		json::get_float(jsonMaterial, "Metallic_Override", m_ShaderCB.metallicAdditive);
		json::get_float(jsonMaterial, "Roughness_Override", m_ShaderCB.roughnessAdditive);

		TextureManager& textureManager = ResourceManager::Get().GetTextureManager();
		m_AlbedoMap = textureManager.GetTextureByID(albedoTexID, Texture::eTextureType::ALBEDO);
		m_NormalMap = textureManager.GetTextureByID(normalTexID, Texture::eTextureType::NORMAL);
		m_MetallicMap = textureManager.GetTextureByID(metallicTexID, Texture::eTextureType::METALLIC);
		m_RoughnessMap = textureManager.GetTextureByID(roughnessTexID, Texture::eTextureType::ROUGHNESS);
		m_AOMap = textureManager.GetTextureByID(aoTexID, Texture::eTextureType::AO);

		return true;
	}

	void Material::OnImGuiRender()
	{
		if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Textures"); 

			ImGui::Text("Albedo:"); ImGui::SameLine();
			ImGui::Text(m_AlbedoMap->GetDisplayName().c_str());

			ImGui::Text("Normal:"); ImGui::SameLine();
			ImGui::Text(m_NormalMap->GetDisplayName().c_str());

			ImGui::Text("Roughness:"); ImGui::SameLine();
			ImGui::Text(m_RoughnessMap->GetDisplayName().c_str());

			ImGui::Text("Metallic:"); ImGui::SameLine();
			ImGui::Text(m_MetallicMap->GetDisplayName().c_str());

			ImGui::Text("AO:"); ImGui::SameLine();
			ImGui::Text(m_AOMap->GetDisplayName().c_str());

			ImGui::Spacing();

			ImGui::Text("PBR Offsets");
			ImGuiColorEditFlags colorWheelFlags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_PickerHueWheel;
			// Imgui will edit the color values in a normalized 0 to 1 space. 
			// In the shaders we transform the color values back into 0 to 255 space.
			ImGui::ColorEdit3("Diffuse Additive: ", &m_ShaderCB.diffuseAdditive.x, colorWheelFlags);
			ImGui::SliderFloat("Metallic Addative", &m_ShaderCB.metallicAdditive, 0.0f, 1.0f);
			ImGui::SliderFloat("Roughness Addative", &m_ShaderCB.roughnessAdditive, 0.0f, 1.0f);

			ImGui::Text("UVs");
			ImGui::DragFloat2("Tiling:", &m_ShaderCB.tiling.x, 0.01f, -50.0f, 50.0f);
			ImGui::DragFloat2("Offset:", &m_ShaderCB.uvOffset.x, 0.01f, -50.0f, 50.0f);
			ImGui::TreePop();
		}

	}

	void Material::BindResources()
	{
		m_AlbedoMap->Bind();
		m_NormalMap->Bind();
		m_MetallicMap->Bind();
		m_RoughnessMap->Bind();
		m_AOMap->Bind();
	}

}