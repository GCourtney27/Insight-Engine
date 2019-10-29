#pragma once
#include "Material.h"

class MaterialUnTextured : public Material
{
public:
	MaterialUnTextured(eMaterialType materialType) { m_materialType = materialType; }

	virtual bool Initiailze(ID3D11Device * device, ID3D11DeviceContext * deviceContext, const rapidjson::Value& assetsInformation, eFlags materialAttributeFlags) override;
	
protected:
	virtual bool InitializePiplineAssets(const rapidjson::Value& assetsInformation) override;

	virtual void InitializeShaders() override;
private:
	std::vector<std::string> m_textureLocations;

};