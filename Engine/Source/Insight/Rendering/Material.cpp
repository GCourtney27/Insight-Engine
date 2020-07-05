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
		// TODO get texture handle by id from textre manager
		const rapidjson::Value& jsonUVOffset = jsonMaterial["uvOffset"];
		const rapidjson::Value& jsonTilingOffset = jsonMaterial["Tiling"];
		const rapidjson::Value& jsonColorOverride = jsonMaterial["Color_Override"];
		json::get_int(jsonMaterial, "AlbedoMapID", m_AlbedoTextureManagerID);
		json::get_int(jsonMaterial, "NormalMapID", m_NormalTextureManagerID);
		json::get_int(jsonMaterial, "MetallicMapID", m_MetallicTextureManagerID);
		json::get_int(jsonMaterial, "RoughnessMapID", m_RoughnessTextureManagerID);
		json::get_int(jsonMaterial, "AOMapID", m_AoTextureManagerID);

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
		m_AlbedoMap = textureManager.GetTextureByID(m_AlbedoTextureManagerID, Texture::eTextureType::ALBEDO);
		m_NormalMap = textureManager.GetTextureByID(m_NormalTextureManagerID, Texture::eTextureType::NORMAL);
		m_MetallicMap = textureManager.GetTextureByID(m_MetallicTextureManagerID, Texture::eTextureType::METALLIC);
		m_RoughnessMap = textureManager.GetTextureByID(m_RoughnessTextureManagerID, Texture::eTextureType::ROUGHNESS);
		m_AOMap = textureManager.GetTextureByID(m_AoTextureManagerID, Texture::eTextureType::AO);

		return true;
	}

	bool Material::WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer)
	{
		// Textures
		{
			Writer.Key("AlbedoMapID");
			Writer.Int(m_AlbedoTextureManagerID);
			Writer.Key("NormalMapID");
			Writer.Int(m_NormalTextureManagerID);
			Writer.Key("MetallicMapID");
			Writer.Int(m_MetallicTextureManagerID);
			Writer.Key("RoughnessMapID");
			Writer.Int(m_RoughnessTextureManagerID);
			Writer.Key("AOMapID");
			Writer.Int(m_AoTextureManagerID);
		}

		// UV's
		{
			Writer.Key("uvOffset");
			Writer.StartArray(); // Start Offsets
			Writer.StartObject();
			{
				Writer.Key("x");
				Writer.Double(m_ShaderCB.uvOffset.x);
				Writer.Key("y");
				Writer.Double(m_ShaderCB.uvOffset.y);
			}
			Writer.EndObject();
			Writer.EndArray(); // End Offsets

			Writer.Key("Tiling");
			Writer.StartArray(); // Start Tiling
			Writer.StartObject();
			{
				Writer.Key("u");
				Writer.Double(m_ShaderCB.tiling.x);
				Writer.Key("v");
				Writer.Double(m_ShaderCB.tiling.y);
			}
			Writer.EndObject();
			Writer.EndArray(); // End Tiling
		}

		// Color Overrides
		{
			Writer.Key("Color_Override");
			Writer.StartArray(); // Start Tiling
			Writer.StartObject();
			{
				Writer.Key("r");
				Writer.Double(m_ShaderCB.diffuseAdditive.x);
				Writer.Key("g");
				Writer.Double(m_ShaderCB.diffuseAdditive.y);
				Writer.Key("b");
				Writer.Double(m_ShaderCB.diffuseAdditive.z);
			}
			Writer.EndObject();
			Writer.EndArray(); // End Tiling
		}

		// PBR Overrides
		{
			Writer.Key("Metallic_Override");
			Writer.Double(m_ShaderCB.metallicAdditive);
			Writer.Key("Roughness_Override");
			Writer.Double(m_ShaderCB.roughnessAdditive);
		}

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