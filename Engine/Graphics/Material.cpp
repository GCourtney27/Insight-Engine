#include "Material.h"
#include "..\Editor\Editor.h"

Material::Material(ID3D11Device * device, ID3D11DeviceContext * deviceContext, eMaterialType materialType, eFlags flags, std::vector<std::string> textureLocations)
{
	this->m_pDevice = device;
	this->m_pDeviceContext = deviceContext;
	this->m_textureLocations = textureLocations;
	this->m_materialType = materialType;
	this->m_flags = flags;
	
	std::vector<std::string>::iterator iter;
	for (iter = textureLocations.begin(); iter != textureLocations.end(); iter++)
	{
		m_textures.push_back(Texture(this->m_pDevice, (*iter)));
	}

	if (!Initialize())
	{
		ErrorLogger::Log("Failed to initilaize material.");
	}
}

bool Material::Initialize()
{
	// Initialize shaders
	// 
	return true;
}

std::string Material::GetMaterialTypeAsString()
{

	std::map<std::string, eMaterialType> stringToMaterialType;
	stringToMaterialType["PBR_MAPPED"] = eMaterialType::PBR_MAPPED;
	stringToMaterialType["PBR_MAPPED_SCALED"] = eMaterialType::PBR_MAPPED_SCALED;
	stringToMaterialType["PBR_DEFAULT"] = eMaterialType::PBR_DEFAULT;
	stringToMaterialType["PBR_SKY"] = eMaterialType::PBR_SKY;

	std::map<std::string, eMaterialType>::iterator iter;
	for (iter = stringToMaterialType.begin(); iter != stringToMaterialType.end(); iter++)
	{
		if ((*iter).second == this->m_materialType)
			return (*iter).first;
	}

	return "ERROR: Could not locate material type";
}

//void Material::SetPixelShader()
//{
//	m_pDeviceContext->PSSetShader(this->m_pixelShader.GetShader(), NULL, 0);
//}
//
//void Material::SetVertexShader()
//{
//	m_pDeviceContext->VSSetShader(this->m_vertexShader.GetShader(), NULL, 0);
//}

Material::eMaterialType Material::GetMaterialTypeFromString(std::string str_material)
{
	std::map<std::string, eMaterialType> stringToMaterialType;

	stringToMaterialType["PBR_MAPPED"] = eMaterialType::PBR_MAPPED;
	stringToMaterialType["PBR_MAPPED_SCALED"] = eMaterialType::PBR_MAPPED_SCALED;
	stringToMaterialType["PBR_DEFAULT"] = eMaterialType::PBR_DEFAULT;
	stringToMaterialType["PBR_SKY"] = eMaterialType::PBR_SKY;

	std::map<std::string, eMaterialType>::iterator iter;
	for (iter = stringToMaterialType.begin(); iter != stringToMaterialType.end(); iter++)
	{
		if ((*iter).first == str_material)
			return (*iter).second;
	}
}
