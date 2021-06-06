#include <Engine_pch.h>

#include "PhysicsManager.h"

#include "Physics/PhysicsCommon.h"


namespace Insight {

	PhysicsManager* PhysicsManager::s_Instance = nullptr;


	PhysicsManager::PhysicsManager()
	{
	}

	PhysicsManager::~PhysicsManager()
	{

	}

	void PhysicsManager::InitGlobalInstance()
	{
		IE_ASSERT(!s_Instance, "An instance of physics manager already exists!");
		s_Instance = new PhysicsManager();
	}

	void PhysicsManager::Simulate(const float DeltaMs)
	{
		
	}

	void PhysicsManager::UnRegisterPhysicsObject(IPhysicsObject* pPhysicsObject)
	{
		auto iter = std::find(s_Instance->m_ScenePhysicsObjects.begin(), s_Instance->m_ScenePhysicsObjects.end(), pPhysicsObject);
		if (iter != s_Instance->m_ScenePhysicsObjects.end())
		{
			s_Instance->m_ScenePhysicsObjects.erase(iter);
		}
	}

}
