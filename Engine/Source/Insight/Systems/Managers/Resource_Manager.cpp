#include <ie_pch.h>

#include "Resource_Manager.h"


namespace Insight {

	ResourceManager* ResourceManager::s_Instance = nullptr;


	ResourceManager::ResourceManager()
	{
		IE_ASSERT(!s_Instance, "An instance of resource manager already exists!");
		s_Instance = this;

		m_pModelManager = new GeometryManager();
		m_pTextureManager = new TextureManager();
		m_pMonoScriptManager = new MonoScriptManager();
	}

	ResourceManager::~ResourceManager()
	{
		delete m_pModelManager;
		delete m_pTextureManager;
		delete m_pMonoScriptManager;
	}

	bool ResourceManager::Init()
	{
		m_pModelManager->Init();
		m_pTextureManager->Init();
		m_pMonoScriptManager->Init();
		return true;
	}

	bool ResourceManager::LoadResourcesFromJson(const rapidjson::Value& jsonResources)
	{
		const rapidjson::Value& jsonTextureResources = jsonResources["Textures"];
		const rapidjson::Value& jsonMeshResources = jsonResources["Meshes"];
		m_pTextureManager->LoadResourcesFromJson(jsonTextureResources);

		return true;
	}

	// Clears all resource caches for the currenly active scene.
	// If used, make sure you are loading a new scene or immediatly 
	// adding new resources AFTER this call
	void ResourceManager::FlushAllResources()
	{
		m_pModelManager->FlushModelCache();
		m_pTextureManager->FlushTextureCache();
		//m_pMonoScriptManager->Cleanup();
	}

}
