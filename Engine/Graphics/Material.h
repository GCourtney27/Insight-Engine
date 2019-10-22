#pragma once
#include "Texture.h"
#include "Shaders.h"
#include <vector>
#include <map>


class Material
{
public:
	static enum eMaterialType
	{
		PBR_MAPPED, // Material is expecting mapps (albedo, roughness, normal, metallic)
		PBR_MAPPED_SCALED, // Material can have values scale its textures (more rough less metallic etc.)
		PBR_DEFAULT, // No Textures all albedo, roughness and metallic are defined through shader inputs fo the material through ImGuiRender
		PBR_SKY // Material only exepts a .dds defining the sky sphere look
		//DEFAULT NO PBR
	};

	static enum eFlags
	{
		NOFLAGS, // Default opaque shader
		FOLIAGE, // Requires opacity culling in shader
		TERRAIN // Requires tessilation shader
	};

public:
	Material(ID3D11Device * device, ID3D11DeviceContext * deviceContext, eMaterialType materialType, eFlags flags, std::vector<std::string> textureLocations);

	static eMaterialType GetMaterialTypeFromString(std::string str_material);

	std::string GetMaterialTypeAsString();
	std::vector<std::string> GetTextureLocations() { return m_textureLocations; }

	//void SetPixelShader();
	//void SetVertexShader();

	std::vector<Texture> m_textures;
protected:
	std::vector<std::string> m_textureLocations;
	bool Initialize();


	eMaterialType m_materialType;
	eFlags m_flags;

	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
};