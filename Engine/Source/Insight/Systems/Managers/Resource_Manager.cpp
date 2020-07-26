#include <ie_pch.h>

#include "Resource_Manager.h"
#include "Platform/Windows/Error/COM_Exception.h"

namespace Insight {

	ResourceManager* ResourceManager::s_Instance = nullptr;


	ResourceManager::ResourceManager()
	{
		IE_ASSERT(!s_Instance, "An instance of resource manager already exists!");
		s_Instance = this;

		//m_pGeometryManager = new GeometryManager();
		m_pTextureManager = new TextureManager();
		m_pMonoScriptManager = new MonoScriptManager();
	}

	ResourceManager::~ResourceManager()
	{
		GeometryManager::Shutdown();
		delete m_pTextureManager;
		delete m_pMonoScriptManager;
	}

	bool ResourceManager::Init()
	{
		GeometryManager::InitGlobalInstance();
		GeometryManager::Init();

		m_pMonoScriptManager->Init();
		m_pTextureManager->Init();
		return true;
	}

	bool ResourceManager::LoadResourcesFromJson(const rapidjson::Value& jsonResources)
	{
		const rapidjson::Value& jsonTextureResources = jsonResources["Textures"];
		m_pTextureManager->LoadResourcesFromJson(jsonTextureResources);

		return true;
	}

	// Clears all resource caches for the currenly active scene.
	// If used, make sure you are loading a new scene or immediatly 
	// adding new resources AFTER this call
	void ResourceManager::FlushAllResources()
	{
		GeometryManager::FlushModelCache();
		m_pTextureManager->FlushTextureCache();
		//m_pMonoScriptManager->Cleanup();
	}

}
