#pragma once

#include <vector>

class RigidBody;

class PhysicsSystem
{
public:
	PhysicsSystem();
	~PhysicsSystem();

	void Simulate(const float& deltaTime);

	void AddEntity(RigidBody* rb);


private:
	std::vector<RigidBody*> m_physicsEntities;

};