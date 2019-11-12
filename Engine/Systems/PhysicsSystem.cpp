#include "PhysicsSystem.h"
#include "..\Components\RigidBodyComponent.h"
#include "..\Objects\Entity.h"
#include "..\Editor\Editor.h"

PhysicsSystem::PhysicsSystem()
{
}

PhysicsSystem::~PhysicsSystem()
{
}

void PhysicsSystem::Simulate(const float& deltaTime)
{
	Entity* currentEntity = nullptr;
	for (RigidBody* cur : m_physicsEntities)
	{
		currentEntity = cur->GetOwner();

		for (RigidBody* testAgainst : m_physicsEntities)
		{
			// If the tags are the same that means it is the 
			// same object dont test against yourself, move on
			if (currentEntity->GetID().GetName() == testAgainst->GetOwner()->GetID().GetName())
				continue;

			double deltaXSquared = currentEntity->GetTransform().GetPosition().x - testAgainst->GetOwner()->GetTransform().GetPosition().x;
			deltaXSquared *= deltaXSquared;
			
			double deltaYSquared = currentEntity->GetTransform().GetPosition().y - testAgainst->GetOwner()->GetTransform().GetPosition().y;
			deltaYSquared *= deltaYSquared;

			double deltaZSquared = currentEntity->GetTransform().GetPosition().z - testAgainst->GetOwner()->GetTransform().GetPosition().z;
			deltaZSquared *= deltaZSquared;

			double sumRadiiSquared = cur->GetRadius() + testAgainst->GetRadius();
			sumRadiiSquared *= sumRadiiSquared;

			if (deltaXSquared + deltaYSquared + deltaZSquared <= sumRadiiSquared)
			{
				DEBUGLOG("Collision");
			}

		}
	}
}

void PhysicsSystem::AddEntity(RigidBody * rb)
{
	m_physicsEntities.push_back(rb);
}
