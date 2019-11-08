#pragma once
#include "Material.h"

class MaterialUnTextured : public Material
{
public:
	MaterialUnTextured(eMaterialType materialType) { m_materialType = materialType; }

	virtual bool Initiailze(ID3D11Device * device, ID3D11DeviceContext * deviceContext, const rapidjson::Value& assetsInformation, eFlags materialAttributeFlags) override;
	bool Initiailze(ID3D11Device * device, ID3D11DeviceContext * deviceContext, eFlags materialAttributeFlags) override;
	void WriteToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;

protected:
	virtual bool InitializeJOSNPiplineAssets(const rapidjson::Value& assetsInformation) override;
	virtual bool InitializePiplineAssets() override;

	virtual void InitializeShaders() override;

	virtual void PSSetShaderResources() override;
private:
	std::vector<std::string> m_textureLocations;

};