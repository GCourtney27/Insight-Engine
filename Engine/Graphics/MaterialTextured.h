#pragma once
#include "Material.h"

class MaterialTextured : public Material
{
public:
	bool Initialize(std::vector<std::string> textureLocations);


	std::vector<std::string> GetTextureLocations() { return m_textureLocations; }

	std::vector<Texture> m_textures;

private:
	std::vector<std::string> m_textureLocations;

	// VertexSahder m_vertexShader;
	// PixelSahder m_pixelShader;
	// ConstantBuffer<CB_VS_vertexshader> constantbuffer_vs_PBR;
	// ConstantBuffer<CB_PS_Light> constantbuffer_ps_PBR;

};
