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
	stringToMaterialType["PBR_DEFAULT"] = eMaterialType::PBR_DEFAULT;
	stringToMaterialType["PBR_UNTEXTURED"] = eMaterialType::PBR_UNTEXTURED;
	stringToMaterialType["PBR_SKY"] = eMaterialType::PBR_SKY;

	std::map<std::string, eMaterialType>::iterator iter;
	for (iter = stringToMaterialType.begin(); iter != stringToMaterialType.end(); iter++)
	{
		if ((*iter).second == this->m_materialType)
			return (*iter).first;
	}

	return "ERROR: Could not locate material type";
}

Material * Material::SetMaterialByType(eMaterialType materialType, eFlags flags)
{
	switch (materialType)
	{
	case eMaterialType::PBR_UNTEXTURED:
	{
		return new MaterialUnTextured(materialType);
	}
		break;
	case eMaterialType::PBR_DEFAULT:
	{
		if (flags == eFlags::FOLIAGE)
			return new MaterialTexturedFoliage(materialType);
		else
			return new MaterialTextured(materialType);
	}
		break;
	case eMaterialType::PBR_SKY:
	{
		return new MaterialSky(materialType);
	}
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

Material::eFlags Material::GetMaterialFlagsFromString(std::string str_flags)
{
	std::map<std::string, eFlags> stringToMaterialFlags;

	stringToMaterialFlags["NOFLAGS"] = eFlags::NOFLAGS;
	stringToMaterialFlags["FOLIAGE"] = eFlags::FOLIAGE;
	stringToMaterialFlags["TERRAIN"] = eFlags::TERRAIN;

	std::map<std::string, eFlags>::iterator iter;
	for (iter = stringToMaterialFlags.begin(); iter != stringToMaterialFlags.end(); iter++)
	{
		if ((*iter).first == str_flags)
			return (*iter).second;
	}
	return eFlags::NOFLAGS;
}

std::string Material::GetMaterialFlagsAsString()
{
	std::map<std::string, eFlags> stringToMaterialFlags;
	stringToMaterialFlags["NOFLAGS"] = eFlags::NOFLAGS;
	stringToMaterialFlags["FOLIAGE"] = eFlags::FOLIAGE;
	stringToMaterialFlags["TERRAIN"] = eFlags::TERRAIN;

	std::map<std::string, eFlags>::iterator iter;
	for (iter = stringToMaterialFlags.begin(); iter != stringToMaterialFlags.end(); iter++)
	{
		if ((*iter).second == this->m_flags)
			return (*iter).first;
	}

	return "ERROR: Could not locate material type";
}

Material::eMaterialType Material::GetMaterialTypeFromString(std::string str_material)
{
	std::map<std::string, eMaterialType> stringToMaterialType;

	stringToMaterialType["PBR_DEFAULT"] = eMaterialType::PBR_DEFAULT;
	stringToMaterialType["PBR_UNTEXTURED"] = eMaterialType::PBR_UNTEXTURED;
	stringToMaterialType["PBR_SKY"] = eMaterialType::PBR_SKY;

	std::map<std::string, eMaterialType>::iterator iter;
	for (iter = stringToMaterialType.begin(); iter != stringToMaterialType.end(); iter++)
	{
		if ((*iter).first == str_material)
			return (*iter).second;
	}
	return eMaterialType::PBR_UNTEXTURED;
}
