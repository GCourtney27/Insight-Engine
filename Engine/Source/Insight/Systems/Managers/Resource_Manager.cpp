#include <ie_pch.h>

#include "Resource_Manager.h"


namespace Insight {

	ResourceManager* ResourceManager::s_Instance = nullptr;



	ResourceManager::ResourceManager()
	{
		IE_ASSERT(!s_Instance, "An instance of resource manager already exists!");
		s_Instance = this;

		m_pModelManager = new ModelManager();
		m_pTextureManager = new TextureManager();
	}

	ResourceManager::~ResourceManager()
	{
		delete m_pModelManager;
	}

	bool ResourceManager::Init()
	{
		m_pModelManager->Init();
		m_pTextureManager->Init();
		return true;
	}

	bool ResourceManager::LoadResourcesFromJson(const rapidjson::Value& jsonResources)
	{
		const rapidjson::Value& jsonTextureResources = jsonResources["Textures"];
		const rapidjson::Value& jsonMeshResources = jsonResources["Meshes"];
		m_pTextureManager->LoadResourcesFromJson(jsonTextureResources);

		return true;
	}

}
