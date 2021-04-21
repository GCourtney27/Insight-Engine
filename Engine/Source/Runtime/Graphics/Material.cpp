#include <Engine_pch.h>

#include "Material.h"

#include "Runtime/Core/Utilities/StringHelper.h"
#include "Runtime/Systems/Managers/ResourceManager.h"

#include "Runtime/UI/UILib.h"


namespace Insight {



	Material::Material()
	{
	}

	Material::Material(std::array<Texture::ID, 5> TextureMangerIds)
	{
		m_MaterialType = EMaterialType::MT_Opaque;

		TextureManager& TextureManager = ResourceManager::Get().GetTextureManager();
		m_AlbedoMap		= TextureMangerIds[0] > 0 ? TextureManager.GetTextureByID(TextureMangerIds[0], Texture::TT_Albedo) : nullptr;
		m_NormalMap		= TextureMangerIds[1] > 0 ? TextureManager.GetTextureByID(TextureMangerIds[1], Texture::TT_Normal) : nullptr;
		m_MetallicMap	= TextureMangerIds[2] > 0 ? TextureManager.GetTextureByID(TextureMangerIds[2], Texture::TT_Metallic) : nullptr;
		m_RoughnessMap	= TextureMangerIds[3] > 0 ? TextureManager.GetTextureByID(TextureMangerIds[3], Texture::TT_Roughness) : nullptr;
		m_AOMap			= TextureMangerIds[4] > 0 ? TextureManager.GetTextureByID(TextureMangerIds[4], Texture::TT_AmbientOcclusion) : nullptr;
		
		// If we got back a default texture, queue the texture for the texture manager
		// to give us the proper texture once it is loaded.
		{
			if (m_AlbedoMap && m_AlbedoMap->IsDefaultTexture()) {
				TextureManager.RegisterTextureLoadCallback(TextureMangerIds[0], &m_AlbedoMap);
			}
			if (m_NormalMap && m_NormalMap->IsDefaultTexture()) {
				TextureManager.RegisterTextureLoadCallback(TextureMangerIds[1], &m_NormalMap);
			}
			if (m_MetallicMap && m_MetallicMap->IsDefaultTexture()) {
				TextureManager.RegisterTextureLoadCallback(TextureMangerIds[2], &m_MetallicMap);
			}
			if (m_RoughnessMap && m_RoughnessMap->IsDefaultTexture()) {
				TextureManager.RegisterTextureLoadCallback(TextureMangerIds[3], &m_RoughnessMap);
			}
			if (m_AOMap && m_AOMap->IsDefaultTexture()) {
				TextureManager.RegisterTextureLoadCallback(TextureMangerIds[4], &m_AOMap);
			}
		}


		m_AlbedoTextureManagerID	= TextureMangerIds[0];
		m_NormalTextureManagerID	= TextureMangerIds[1];
		m_MetallicTextureManagerID	= TextureMangerIds[2];
		m_RoughnessTextureManagerID = TextureMangerIds[3];
		m_AoTextureManagerID		= TextureMangerIds[4];

		m_MaterialType					= EMaterialType::MT_Opaque;
		m_ShaderCB.DiffuseAdditive		= FVector3(0.0f, 0.0f, 0.0f);
		m_ShaderCB.MetallicAdditive		= 0.0f;
		m_ShaderCB.RoughnessAdditive	= 0.0f;
		m_ShaderCB.UVTiling				= FVector2(1.0f, 1.0f);
		m_ShaderCB.UVOffset				= FVector2(0.0f, 0.0f);
		m_ShaderCB.Specular				= 0.04f;
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

		pMaterial->m_MaterialType = EMaterialType::MT_Opaque;

		TextureManager& TextureManager = ResourceManager::Get().GetTextureManager();

		pMaterial->m_AlbedoMap		= TextureManager.GetDefaultAlbedoTexture();
		pMaterial->m_NormalMap		= TextureManager.GetDefaultNormalTexture();
		pMaterial->m_MetallicMap	= TextureManager.GetDefaultMetallicTexture();
		pMaterial->m_RoughnessMap	= TextureManager.GetDefaultRoughnessTexture();
		pMaterial->m_AOMap			= TextureManager.GetDefaultAOTexture();

		pMaterial->m_AlbedoTextureManagerID		= pMaterial->m_AlbedoMap->GetTextureInfo().Id;
		pMaterial->m_NormalTextureManagerID		= pMaterial->m_NormalMap->GetTextureInfo().Id;
		pMaterial->m_MetallicTextureManagerID	= pMaterial->m_MetallicMap->GetTextureInfo().Id;
		pMaterial->m_RoughnessTextureManagerID	= pMaterial->m_RoughnessMap->GetTextureInfo().Id;
		pMaterial->m_AoTextureManagerID			= pMaterial->m_AOMap->GetTextureInfo().Id;

		pMaterial->m_MaterialType				= EMaterialType::MT_Opaque;
		pMaterial->m_ShaderCB.DiffuseAdditive	= FVector3(0.0f, 0.0f, 0.0f);
		pMaterial->m_ShaderCB.MetallicAdditive	= 0.0f;
		pMaterial->m_ShaderCB.RoughnessAdditive = 0.0f;
		pMaterial->m_ShaderCB.UVTiling			= FVector2(5.0f, 5.0f);
		pMaterial->m_ShaderCB.UVOffset			= FVector2(0.0f, 0.0f);
		pMaterial->m_ShaderCB.Specular			= 0.04f;

		return pMaterial;
	}

	bool Material::LoadFromJson(const rapidjson::Value& JsonMaterial)
	{
		const rapidjson::Value& jsonUVOffset = JsonMaterial["uvOffset"];
		const rapidjson::Value& jsonTilingOffset = JsonMaterial["Tiling"];
		const rapidjson::Value& jsonColorOverride = JsonMaterial["Color_Override"];

		json::get_int(JsonMaterial, "Category", *((int*)&m_MaterialType));

		if (m_MaterialType == EMaterialType::MT_Opaque) {

			json::get_int(JsonMaterial, "AlbedoMapID", m_AlbedoTextureManagerID);
			json::get_int(JsonMaterial, "NormalMapID", m_NormalTextureManagerID);
			json::get_int(JsonMaterial, "MetallicMapID", m_MetallicTextureManagerID);
			json::get_int(JsonMaterial, "RoughnessMapID", m_RoughnessTextureManagerID);
			json::get_int(JsonMaterial, "AOMapID", m_AoTextureManagerID);

			TextureManager& textureManager = ResourceManager::Get().GetTextureManager();
			m_AlbedoMap = textureManager.GetTextureByID(m_AlbedoTextureManagerID, Texture::ETextureType::TT_Albedo);
			m_NormalMap = textureManager.GetTextureByID(m_NormalTextureManagerID, Texture::ETextureType::TT_Normal);
			m_MetallicMap = textureManager.GetTextureByID(m_MetallicTextureManagerID, Texture::ETextureType::TT_Metallic);
			m_RoughnessMap = textureManager.GetTextureByID(m_RoughnessTextureManagerID, Texture::ETextureType::TT_Roughness);
			m_AOMap = textureManager.GetTextureByID(m_AoTextureManagerID, Texture::ETextureType::TT_AmbientOcclusion);

		} if (m_MaterialType == EMaterialType::MT_Translucent) {

			json::get_int(JsonMaterial, "AlbedoMapID", m_AlbedoTextureManagerID);
			json::get_int(JsonMaterial, "NormalMapID", m_NormalTextureManagerID);
			json::get_int(JsonMaterial, "RoughnessMapID", m_RoughnessTextureManagerID);
			json::get_int(JsonMaterial, "OpacityMapID", m_OpacityTextureManagerID);
			json::get_int(JsonMaterial, "TranslucencyMapID", m_TranslucencyTextureManagerID);

			TextureManager& textureManager = ResourceManager::Get().GetTextureManager();
			m_AlbedoMap = textureManager.GetTextureByID(m_AlbedoTextureManagerID, Texture::ETextureType::TT_Albedo);
			m_NormalMap = textureManager.GetTextureByID(m_NormalTextureManagerID, Texture::ETextureType::TT_Normal);
			m_RoughnessMap = textureManager.GetTextureByID(m_RoughnessTextureManagerID, Texture::ETextureType::TT_Roughness);
			m_OpacityMap = textureManager.GetTextureByID(m_OpacityTextureManagerID, Texture::ETextureType::TT_Opacity);
			m_TranslucencyMap = textureManager.GetTextureByID(m_TranslucencyTextureManagerID, Texture::ETextureType::TT_Translucency);
		}

		json::get_float(jsonUVOffset[0], "x", m_ShaderCB.UVOffset.x);
		json::get_float(jsonUVOffset[0], "y", m_ShaderCB.UVOffset.y);

		json::get_float(jsonTilingOffset[0], "u", m_ShaderCB.UVTiling.x);
		json::get_float(jsonTilingOffset[0], "v", m_ShaderCB.UVTiling.y);

		json::get_float(jsonColorOverride[0], "r", m_ShaderCB.DiffuseAdditive.x);
		json::get_float(jsonColorOverride[0], "g", m_ShaderCB.DiffuseAdditive.y);
		json::get_float(jsonColorOverride[0], "b", m_ShaderCB.DiffuseAdditive.z);

		json::get_float(JsonMaterial, "Metallic_Override", m_ShaderCB.MetallicAdditive);
		json::get_float(JsonMaterial, "Roughness_Override", m_ShaderCB.RoughnessAdditive);

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
			if (m_MaterialType == EMaterialType::MT_Opaque) {
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
			else if (m_MaterialType == EMaterialType::MT_Translucent) {
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
				Writer.Double(m_ShaderCB.UVOffset.x);
				Writer.Key("y");
				Writer.Double(m_ShaderCB.UVOffset.y);
			}
			Writer.EndObject();
			Writer.EndArray(); // End Offsets

			Writer.Key("Tiling");
			Writer.StartArray(); // Start Tiling
			Writer.StartObject();
			{
				Writer.Key("u");
				Writer.Double(m_ShaderCB.UVTiling.x);
				Writer.Key("v");
				Writer.Double(m_ShaderCB.UVTiling.y);
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
				Writer.Double(m_ShaderCB.DiffuseAdditive.x);
				Writer.Key("g");
				Writer.Double(m_ShaderCB.DiffuseAdditive.y);
				Writer.Key("b");
				Writer.Double(m_ShaderCB.DiffuseAdditive.z);
			}
			Writer.EndObject();
			Writer.EndArray(); // End Tiling
		}

		// PBR Overrides
		{
			Writer.Key("Metallic_Override");
			Writer.Double(m_ShaderCB.MetallicAdditive);
			Writer.Key("Roughness_Override");
			Writer.Double(m_ShaderCB.RoughnessAdditive);
		}

		return true;
	}

	void Material::OnImGuiRender()
	{
		if (UI::TreeNodeEx("Material", UI::TreeNode_DefaultOpen))
		{
			UI::Text("PBR Offsets");
			constexpr UI::ColorPickerFlags colorWheelFlags = UI::ColorPickerFlags_NoAlpha | UI::ColorPickerFlags_Uint8 | UI::ColorPickerFlags_PickerHueWheel;

			// Imgui will edit the color values in a normalized 0 to 1 space. 
			// In the shaders we transform the color values back into 0 to 255 space.
			UI::ColorPicker3("Diffuse Additive: ", &m_ShaderCB.DiffuseAdditive.x, colorWheelFlags);
			UI::SliderFloat("Metallic Addative", &m_ShaderCB.MetallicAdditive, -1.0f, 1.0f);
			UI::SliderFloat("Roughness Addative", &m_ShaderCB.RoughnessAdditive, -1.0f, 1.0f);
			//UI::SliderFloat("Specular Addative", &m_ShaderCB.Specular, -1.0f, 1.0f, "%.3f", 0.01f);
			UI::DragFloat("Specular Addative", &m_ShaderCB.Specular, 0.01f, 0.0f, 0.8f);
			UI::Text("UVs");
			UI::DragFloat2("Tiling:", &m_ShaderCB.UVTiling.x, 0.01f, -50.0f, 50.0f);
			UI::DragFloat2("Offset:", &m_ShaderCB.UVOffset.x, 0.01f, -50.0f, 50.0f);
			UI::TreePopNode();
		}

	}

	void Material::BindResources(bool IsDeferredPass)
	{
		if (m_AlbedoMap.get()) {
			if (IsDeferredPass) {
				m_AlbedoMap->BindForDeferredPass();
			}
			else {
				m_AlbedoMap->BindForForwardPass();
			}
		}

		if (m_NormalMap.get()) {
			if (IsDeferredPass) {
				m_NormalMap->BindForDeferredPass();
			}
			else {
				m_NormalMap->BindForForwardPass();
			}
		}

		if (m_MetallicMap.get()) {
			if (IsDeferredPass) {
				m_MetallicMap->BindForDeferredPass();
			}
			else {
				m_MetallicMap->BindForForwardPass();
			}
		}

		if (m_RoughnessMap.get()) {
			if (IsDeferredPass) {
				m_RoughnessMap->BindForDeferredPass();
			}
			else {
				m_RoughnessMap->BindForForwardPass();
			}
		}

		if (m_AOMap.get()) {
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