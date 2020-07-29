#pragma once

#include <Insight/Core.h>

#include "Insight/Core/Interfaces.h"

namespace Insight {

	class INSIGHT_API PhysicsManager
	{
	public:
		PhysicsManager();
		~PhysicsManager();
		
		void Simulate(const float DeltaMs);

		static void RegisterPhysicsObject(IPhysicsObject* pPhysicsObject) { s_Instance->m_ScenePhysicsObjects.push_back(pPhysicsObject); }
		static void UnRegisterPhysicsObject(IPhysicsObject* pPhysicsObject);

	private:
		std::vector<IPhysicsObject*> m_ScenePhysicsObjects;
	private:
		static PhysicsManager* s_Instance;
	};

}
