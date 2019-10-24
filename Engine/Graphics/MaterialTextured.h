#pragma once
#include "Material.h"

class MaterialTextured : public Material
{
public:
	MaterialTextured(eMaterialType materialType) { m_materialType = materialType; }
	virtual bool Initiailze(ID3D11Device * device, ID3D11DeviceContext * deviceContext, const rapidjson::Value& assetsInformation, eFlags materialAttributeFlags) override;


	std::vector<std::string> GetTextureLocations() { return m_textureLocations; }

protected:
	virtual bool InitializePiplineAssets(const rapidjson::Value& componentInformation) override;

	virtual void InitializeShaders() override;

private:
	std::vector<std::string> m_textureLocations;


	// ConstantBuffer<CB_VS_vertexshader> constantbuffer_vs_PBR;
	// ConstantBuffer<CB_PS_Light> constantbuffer_ps_PBR;

};
