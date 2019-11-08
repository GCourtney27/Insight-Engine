#pragma once
#include "Material.h"

class MaterialTexturedFoliage : public Material
{
public:
	MaterialTexturedFoliage(eMaterialType materialType) { m_materialType = materialType; }
	virtual bool Initiailze(ID3D11Device * device, ID3D11DeviceContext * deviceContext, const rapidjson::Value& assetsInformation, eFlags materialAttributeFlags) override;
	bool Initiailze(ID3D11Device * device, ID3D11DeviceContext * deviceContext, eFlags materialAttributeFlags) override;
	void WriteToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;

	std::vector<std::string> GetTextureLocations() { return m_textureLocations; }

protected:
	virtual bool InitializeJOSNPiplineAssets(const rapidjson::Value& componentInformation) override;
	virtual bool InitializePiplineAssets() override;

	virtual void InitializeShaders() override;

	virtual void PSSetShaderResources() override;
private:
	std::vector<std::string> m_textureLocations;

	// -- Initialize shaders in Initialize shader function -- //
	// VertexShader m_vertexShader; // Make new shader that accepts the new constant buffer that is declared below
	// PixelShader m_pixelShader; // Make new shader that accepts PBR textures and opacity map
	// ConstantBuffer<CB_VS_Foliage> constantbuffer_vs_Foliage; // Make new constant buffer to accept delta time in vertex shader
	// ConstantBuffer<CB_PS_Foliage> constantbuffer_ps_Foliage;
};

