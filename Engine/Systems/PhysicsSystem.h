#pragma once

#include <vector>

class RigidBody;
struct Contact;

class PhysicsSystem
{
public:
	PhysicsSystem();
	~PhysicsSystem();

	void Simulate(const float& deltaTime);
	void AddEntity(RigidBody* rb);

private:
	void ProcessContacts();

private:
	std::vector<RigidBody*> m_physicsEntities;
	std::vector<Contact> m_contacts;
};