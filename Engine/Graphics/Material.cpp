#include "Material.h"
#include "..\Editor\Editor.h"

Material::Material(ID3D11Device * device, ID3D11DeviceContext * deviceContext, std::string materialType,  std::vector<std::string> textureLocations)
{
	DetermineMaterialType(materialType);
	this->m_pDevice = device;
	this->m_pDeviceContext = deviceContext;
	this->m_textureLocations = textureLocations;

	std::vector<std::string>::iterator iter;
	for (iter = textureLocations.begin(); iter != textureLocations.end(); iter++)
	{
		m_textures.push_back(Texture(device, (*iter)));
	}
	
}

std::string Material::GetMaterialTypeAsString()
{

	std::map<std::string, eMaterialType> stringToMaterialType;
	stringToMaterialType["PBR_MAPPED"] = eMaterialType::PBR_MAPPED;
	stringToMaterialType["PBR_MAPPED_SCALED"] = eMaterialType::PBR_MAPPED_SCALED;
	stringToMaterialType["PBR_DEFAULT"] = eMaterialType::PBR_MAPPED;
	stringToMaterialType["PBR_SKY"] = eMaterialType::PBR_SKY;

	std::map<std::string, eMaterialType>::iterator iter;
	for (iter = stringToMaterialType.begin(); iter != stringToMaterialType.end(); iter++)
	{
		if ((*iter).second == this->m_materialType)
			return (*iter).first;
	}

	return "";
}

void Material::DetermineMaterialType(std::string str_material)
{
	std::map<std::string, eMaterialType> stringToMaterialType;

	stringToMaterialType["PBR_MAPPED"] = eMaterialType::PBR_MAPPED;
	stringToMaterialType["PBR_MAPPED_SCALED"] = eMaterialType::PBR_MAPPED_SCALED;
	stringToMaterialType["PBR_DEFAULT"] = eMaterialType::PBR_MAPPED;
	stringToMaterialType["PBR_SKY"] = eMaterialType::PBR_SKY;

	std::map<std::string, eMaterialType>::iterator iter;
	for (iter = stringToMaterialType.begin(); iter != stringToMaterialType.end(); iter++)
	{
		if ((*iter).first == str_material)
			m_materialType = (*iter).second;
	}
}
