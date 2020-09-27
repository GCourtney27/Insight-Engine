#pragma once

#include <Retina/Core.h>


namespace Retina {

	class IPhysicsObject;

	class RETINA_API PhysicsManager
	{
	public:
		PhysicsManager();
		~PhysicsManager();
		
		static void InitGlobalInstance();
		static void Simulate(const float DeltaMs);

		static void RegisterPhysicsObject(IPhysicsObject* pPhysicsObject) { s_Instance->m_ScenePhysicsObjects.push_back(pPhysicsObject); }
		static void UnRegisterPhysicsObject(IPhysicsObject* pPhysicsObject);

	private:
		std::vector<IPhysicsObject*> m_ScenePhysicsObjects;
	private:
		static PhysicsManager* s_Instance;
	};

}
