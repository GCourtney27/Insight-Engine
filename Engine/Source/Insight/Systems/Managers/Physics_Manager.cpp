#include <ie_pch.h>

#include "Physics_Manager.h"

namespace Insight {

	PhysicsManager* PhysicsManager::s_Instance = nullptr;


	PhysicsManager::PhysicsManager()
	{
		IE_ASSERT(!s_Instance, "An instance of physics manager already exists!");
		s_Instance = this;
	}

	PhysicsManager::~PhysicsManager()
	{

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
