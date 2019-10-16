#pragma once
#include "SimpleShader.h"
#include "Texture.h"
#include "Shaders.h"
#include <map>

class Material
{
public:
	enum eMaterialType
	{
		PBR_MAPPED, // Material is expecting mapps (albedo, roughness, normal, metallic)
		PBR_MAPPED_SCALED, // Material can have values scale its textures (more rough less metallic etc.)
		PBR_DEFAULT, // No Textures all albedo, roughness and metallic are defined through shader inputs fo the material through ImGuiRender
		PBR_SKY // Material only exepts a .dds defining the sky sphere look
	};
private:
	void DetermineMaterialType(std::string str_material);
public:
	Material(ID3D11Device * device, ID3D11DeviceContext * deviceContext, std::string materialType, std::vector<std::string> textureLocations);

	std::string GetMaterialTypeAsString();
	std::vector<std::string> GetTextureLocations() { return m_textureLocations; }

	std::vector<Texture> m_textures;
private:
	std::vector<std::string> m_textureLocations;

	std::vector<Shader> m_shaders;
	eMaterialType m_materialType;

	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
};
