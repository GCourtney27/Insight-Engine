#include "Material.h"
#include "..\Editor\Editor.h"

#include "MaterialUnTextured.h"
#include "MaterialTextured.h"
#include "MaterialTexturedFoliage.h"
#include "MaterialSky.h"

#include "Graphics.h"

std::string Material::GetMaterialTypeAsString()
{
	std::map<std::string, eMaterialType> stringToMaterialType;
	stringToMaterialType["PBR_MAPPED"] = eMaterialType::PBR_MAPPED;
	stringToMaterialType["PBR_MAPPED_WITHSCALERS"] = eMaterialType::PBR_MAPPED_WITHSCALERS;
	stringToMaterialType["PBR_DEFAULT"] = eMaterialType::PBR_UNTEXTURED;
	stringToMaterialType["PBR_SKY"] = eMaterialType::PBR_SKY;

	std::map<std::string, eMaterialType>::iterator iter;
	for (iter = stringToMaterialType.begin(); iter != stringToMaterialType.end(); iter++)
	{
		if ((*iter).second == this->m_materialType)
			return (*iter).first;
	}

	return "ERROR: Could not locate material type";
}

Material * Material::SetMaterialByType(eMaterialType materialType)
{
	switch (materialType)
	{
	case eMaterialType::PBR_UNTEXTURED:
		return new MaterialUnTextured(materialType);
		break;
	case eMaterialType::PBR_MAPPED:
		return new MaterialTextured(materialType);
		break;
	case eMaterialType::PBR_SKY:
		return new MaterialSky(materialType);
		break;
	case eMaterialType::PBR_MAPPED_WITHSCALERS:
		//return new MaterialTexturedScalable(materialType);
		break;
	default:
		ErrorLogger::Log("Could not determine proper material based on eMaterialType");
		break;
	}
	return nullptr;
}

void Material::PSSetShader()
{
	m_pDeviceContext->PSSetShader(this->m_pixelShader.GetShader(), NULL, 0);
}

void Material::VSSetShader()
{
	m_pDeviceContext->VSSetShader(this->m_vertexShader.GetShader(), NULL, 0);
}

void Material::IASetInputLayout()
{
	m_pDeviceContext->IASetInputLayout(m_vertexShader.GetInputLayout());
}

Material::eMaterialType Material::GetMaterialTypeFromString(std::string str_material)
{
	std::map<std::string, eMaterialType> stringToMaterialType;

	stringToMaterialType["PBR_MAPPED"] = eMaterialType::PBR_MAPPED;
	stringToMaterialType["PBR_MAPPED_WITHSCALERS"] = eMaterialType::PBR_MAPPED_WITHSCALERS;
	stringToMaterialType["PBR_DEFAULT"] = eMaterialType::PBR_UNTEXTURED;
	stringToMaterialType["PBR_SKY"] = eMaterialType::PBR_SKY;

	std::map<std::string, eMaterialType>::iterator iter;
	for (iter = stringToMaterialType.begin(); iter != stringToMaterialType.end(); iter++)
	{
		if ((*iter).first == str_material)
			return (*iter).second;
	}
	return eMaterialType::PBR_UNTEXTURED;
}
