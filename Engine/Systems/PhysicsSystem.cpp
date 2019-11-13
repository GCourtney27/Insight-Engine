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
		if (!cur->GetIsComponentEnabled())
			continue;
		currentEntity = cur->GetOwner();

		for (RigidBody* testAgainst : m_physicsEntities)
		{
			// If the tags are the same that means it is the 
			// same object dont test against yourself, move on
			if (currentEntity->GetID().GetName() == testAgainst->GetOwner()->GetID().GetName())
				continue;

			if (!testAgainst->GetIsComponentEnabled())
				continue;

			if (cur->GetCollider().Intersects(&testAgainst->GetCollider()))
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
