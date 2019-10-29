#include "MaterialSky.h"

bool MaterialSky::Initiailze(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const rapidjson::Value & assetsInformation, eFlags materialAttributeFlags)
{
	this->m_pDevice = pDevice;
	this->m_pDeviceContext = pDeviceContext;
	this->m_flags = materialAttributeFlags;
	if (!InitializePiplineAssets(assetsInformation))
	{
		ErrorLogger::Log("Failed to initialize pipline assets for skybox material.");
		return false;
	}
	
	return true;
}

bool MaterialSky::InitializePiplineAssets(const rapidjson::Value & assetsInformation)
{

	return true;
}

void MaterialSky::InitializeShaders()
{

}
