#include <Engine_pch.h>

#include "Material.h"

#include "Insight/Utilities/String_Helper.h"
#include "Insight/Systems/File_System.h"
#include "Insight/Systems/Managers/Resource_Manager.h"

#include "imgui.h"

namespace Insight {



	Material::Material()
	{
	}

	Material::Material(std::array<Texture::ID, 5> TextureMangerIds)
	{
		m_MaterialType = eMaterialType::eMaterialType_Opaque;

		TextureManager& TextureManager = ResourceManager::Get().GetTextureManager();
		m_AlbedoMap = TextureManager.GetTextureByID(TextureMangerIds[0], Texture::eTextureType_Albedo);
		m_NormalMap = TextureManager.GetTextureByID(TextureMangerIds[1], Texture::eTextureType_Normal);
		m_MetallicMap = TextureManager.GetTextureByID(TextureMangerIds[2], Texture::eTextureType_Metallic);
		m_RoughnessMap = TextureManager.GetTextureByID(TextureMangerIds[3], Texture::eTextureType_Roughness);
		m_AOMap = TextureManager.GetTextureByID(TextureMangerIds[4], Texture::eTextureType_AmbientOcclusion);
		
		m_AlbedoTextureManagerID = TextureMangerIds[0];
		m_NormalTextureManagerID = TextureMangerIds[1];
		m_MetallicTextureManagerID = TextureMangerIds[2];
		m_RoughnessTextureManagerID = TextureMangerIds[3];
		m_AoTextureManagerID = TextureMangerIds[4];

		m_MaterialType = eMaterialType::eMaterialType_Opaque;
		m_ShaderCB.diffuseAdditive = ieVector3(0.0f, 0.0f, 0.0f);
		m_ShaderCB.metallicAdditive = 0.0f;
		m_ShaderCB.roughnessAdditive = 0.0f;
		m_ShaderCB.tiling = ieVector2(1.0f, 1.0f);
		m_ShaderCB.uvOffset = ieVector2(0.0f, 0.0f);
	}

	Material::Material(Material&& material) noexcept
	{
		m_AlbedoMap = std::move(material.m_AlbedoMap);
		m_NormalMap = std::move(material.m_NormalMap);
		m_MetallicMap = std::move(material.m_RoughnessMap);
		m_RoughnessMap = std::move(material.m_RoughnessMap);
		m_AOMap = std::move(material.m_AOMap);

		m_UVOffset = std::move(material.m_UVOffset);
		m_Tiling = std::move(material.m_Tiling);
		m_ColorAdditive = std::move(material.m_ColorAdditive);

		m_AlbedoMap = nullptr;
		m_NormalMap = nullptr;
		m_MetallicMap = nullptr;
		m_RoughnessMap = nullptr;
		m_AOMap = nullptr;
	}

	Material::~Material()
	{
		Destroy();
	}

	void Material::Destroy()
	{

	}

	Material* Material::CreateDefaultTexturedMaterial()
	{
		Material* pMaterial = new Material();

		pMaterial->m_MaterialType = eMaterialType::eMaterialType_Opaque;

		TextureManager& TextureManager = ResourceManager::Get().GetTextureManager();

		pMaterial->m_AlbedoMap = TextureManager.GetDefaultAlbedoTexture();
		pMaterial->m_NormalMap = TextureManager.GetDefaultNormalTexture();
		pMaterial->m_MetallicMap = TextureManager.GetDefaultMetallicTexture();
		pMaterial->m_RoughnessMap = TextureManager.GetDefaultRoughnessTexture();
		pMaterial->m_AOMap = TextureManager.GetDefaultAOTexture();

		pMaterial->m_AlbedoTextureManagerID = pMaterial->m_AlbedoMap->GetTextureInfo().Id;
		pMaterial->m_NormalTextureManagerID = pMaterial->m_NormalMap->GetTextureInfo().Id;
		pMaterial->m_MetallicTextureManagerID = pMaterial->m_MetallicMap->GetTextureInfo().Id;
		pMaterial->m_RoughnessTextureManagerID = pMaterial->m_RoughnessMap->GetTextureInfo().Id;
		pMaterial->m_AoTextureManagerID = pMaterial->m_AOMap->GetTextureInfo().Id;

		pMaterial->m_MaterialType = eMaterialType::eMaterialType_Opaque;
		pMaterial->m_ShaderCB.diffuseAdditive = ieVector3(0.0f, 0.0f, 0.0f);
		pMaterial->m_ShaderCB.metallicAdditive = 0.0f;
		pMaterial->m_ShaderCB.roughnessAdditive = 0.0f;
		pMaterial->m_ShaderCB.tiling = ieVector2(1.0f, 1.0f);
		pMaterial->m_ShaderCB.uvOffset = ieVector2(0.0f, 0.0f);

		return pMaterial;
	}

	bool Material::LoadFromJson(const rapidjson::Value& JsonMaterial)
	{
		const rapidjson::Value& jsonUVOffset = JsonMaterial["uvOffset"];
		const rapidjson::Value& jsonTilingOffset = JsonMaterial["Tiling"];
		const rapidjson::Value& jsonColorOverride = JsonMaterial["Color_Override"];

		json::get_int(JsonMaterial, "Category", *((int*)&m_MaterialType));

		if (m_MaterialType == eMaterialType::eMaterialType_Opaque) {

			json::get_int(JsonMaterial, "AlbedoMapID", m_AlbedoTextureManagerID);
			json::get_int(JsonMaterial, "NormalMapID", m_NormalTextureManagerID);
			json::get_int(JsonMaterial, "MetallicMapID", m_MetallicTextureManagerID);
			json::get_int(JsonMaterial, "RoughnessMapID", m_RoughnessTextureManagerID);
			json::get_int(JsonMaterial, "AOMapID", m_AoTextureManagerID);

			TextureManager& textureManager = ResourceManager::Get().GetTextureManager();
			m_AlbedoMap = textureManager.GetTextureByID(m_AlbedoTextureManagerID, Texture::eTextureType::eTextureType_Albedo);
			m_NormalMap = textureManager.GetTextureByID(m_NormalTextureManagerID, Texture::eTextureType::eTextureType_Normal);
			m_MetallicMap = textureManager.GetTextureByID(m_MetallicTextureManagerID, Texture::eTextureType::eTextureType_Metallic);
			m_RoughnessMap = textureManager.GetTextureByID(m_RoughnessTextureManagerID, Texture::eTextureType::eTextureType_Roughness);
			m_AOMap = textureManager.GetTextureByID(m_AoTextureManagerID, Texture::eTextureType::eTextureType_AmbientOcclusion);

		} if (m_MaterialType == eMaterialType::eMaterialType_Translucent) {

			json::get_int(JsonMaterial, "AlbedoMapID", m_AlbedoTextureManagerID);
			json::get_int(JsonMaterial, "NormalMapID", m_NormalTextureManagerID);
			json::get_int(JsonMaterial, "RoughnessMapID", m_RoughnessTextureManagerID);
			json::get_int(JsonMaterial, "OpacityMapID", m_OpacityTextureManagerID);
			json::get_int(JsonMaterial, "TranslucencyMapID", m_TranslucencyTextureManagerID);

			TextureManager& textureManager = ResourceManager::Get().GetTextureManager();
			m_AlbedoMap = textureManager.GetTextureByID(m_AlbedoTextureManagerID, Texture::eTextureType::eTextureType_Albedo);
			m_NormalMap = textureManager.GetTextureByID(m_NormalTextureManagerID, Texture::eTextureType::eTextureType_Normal);
			m_RoughnessMap = textureManager.GetTextureByID(m_RoughnessTextureManagerID, Texture::eTextureType::eTextureType_Roughness);
			m_OpacityMap = textureManager.GetTextureByID(m_OpacityTextureManagerID, Texture::eTextureType::eTextureType_Opacity);
			m_TranslucencyMap = textureManager.GetTextureByID(m_TranslucencyTextureManagerID, Texture::eTextureType::eTextureType_Translucency);
		}

		json::get_float(jsonUVOffset[0], "x", m_ShaderCB.uvOffset.x);
		json::get_float(jsonUVOffset[0], "y", m_ShaderCB.uvOffset.y);

		json::get_float(jsonTilingOffset[0], "u", m_ShaderCB.tiling.x);
		json::get_float(jsonTilingOffset[0], "v", m_ShaderCB.tiling.y);

		json::get_float(jsonColorOverride[0], "r", m_ShaderCB.diffuseAdditive.x);
		json::get_float(jsonColorOverride[0], "g", m_ShaderCB.diffuseAdditive.y);
		json::get_float(jsonColorOverride[0], "b", m_ShaderCB.diffuseAdditive.z);

		json::get_float(JsonMaterial, "Metallic_Override", m_ShaderCB.metallicAdditive);
		json::get_float(JsonMaterial, "Roughness_Override", m_ShaderCB.roughnessAdditive);

		return true;
	}

	bool Material::WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer)
	{
		// Material Properties
		{
			Writer.Key("Category");
			Writer.Int(m_MaterialType);
		}

		// Textures
		{
			if (m_MaterialType == eMaterialType::eMaterialType_Opaque) {
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
			else if (m_MaterialType == eMaterialType::eMaterialType_Translucent) {
				Writer.Key("AlbedoMapID");
				Writer.Int(m_AlbedoTextureManagerID);
				Writer.Key("NormalMapID");
				Writer.Int(m_NormalTextureManagerID);
				Writer.Key("RoughnessMapID");
				Writer.Int(m_RoughnessTextureManagerID);
				Writer.Key("OpacityMapID");
				Writer.Int(m_OpacityTextureManagerID);
				Writer.Key("TranslucencyMapID");
				Writer.Int(m_TranslucencyTextureManagerID);
			}
			
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
			//ImGui::Text(m_AlbedoMap->GetDisplayName().c_str());

			ImGui::Text("Normal:"); ImGui::SameLine();
			//ImGui::Text(m_NormalMap->GetDisplayName().c_str());

			ImGui::Text("Roughness:"); ImGui::SameLine();
			//ImGui::Text(m_RoughnessMap->GetDisplayName().c_str());

			ImGui::Text("Metallic:"); ImGui::SameLine();
			//ImGui::Text(m_MetallicMap->GetDisplayName().c_str());

			ImGui::Text("AO:"); ImGui::SameLine();
			//ImGui::Text(m_AOMap->GetDisplayName().c_str());

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

	void Material::BindResources(bool IsDeferredPass)
	{
		if (m_AlbedoMap.get()) {
			if (m_AlbedoMap->GetTextureInfo().Id == -1) {
				m_AlbedoMap = ResourceManager::Get().GetTextureManager().GetTextureByID(m_AlbedoTextureManagerID, Texture::eTextureType_Albedo);
			}
			if (IsDeferredPass) {
				m_AlbedoMap->BindForDeferredPass();
			}
			else {
				m_AlbedoMap->BindForForwardPass();
			}
		}

		if (m_NormalMap.get()) {
			if (m_NormalMap->GetTextureInfo().Id == -2) {
				m_NormalMap = ResourceManager::Get().GetTextureManager().GetTextureByID(m_NormalTextureManagerID, Texture::eTextureType_Normal);
			}
			if (IsDeferredPass) {
				m_NormalMap->BindForDeferredPass();
			}
			else {
				m_NormalMap->BindForForwardPass();
			}
		}

		if (m_MetallicMap.get()) {
			if (m_MetallicMap->GetTextureInfo().Id == -3) {
				m_MetallicMap = ResourceManager::Get().GetTextureManager().GetTextureByID(m_MetallicTextureManagerID, Texture::eTextureType_Metallic);
			}
			if (IsDeferredPass) {
				m_MetallicMap->BindForDeferredPass();
			}
			else {
				m_MetallicMap->BindForForwardPass();
			}
		}

		if (m_RoughnessMap.get()) {
			if (m_RoughnessMap->GetTextureInfo().Id == -4) {
				m_RoughnessMap = ResourceManager::Get().GetTextureManager().GetTextureByID(m_RoughnessTextureManagerID, Texture::eTextureType_Roughness);
			}
			if (IsDeferredPass) {
				m_RoughnessMap->BindForDeferredPass();
			}
			else {
				m_RoughnessMap->BindForForwardPass();
			}
		}

		if (m_AOMap.get()) {
			if (m_AOMap->GetTextureInfo().Id == -4) {
				m_AOMap = ResourceManager::Get().GetTextureManager().GetTextureByID(m_AoTextureManagerID, Texture::eTextureType_AmbientOcclusion);
			}
			if (IsDeferredPass) {
				m_AOMap->BindForDeferredPass();
			}
			else {
				m_AOMap->BindForForwardPass();
			}
		}

		if (m_OpacityMap.get()) {
			m_OpacityMap->BindForForwardPass();
		}

		if (m_TranslucencyMap.get()) {
			m_TranslucencyMap->BindForForwardPass();
		}
	}

}