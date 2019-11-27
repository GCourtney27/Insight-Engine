#include "PhysicsSystem.h"
#include "..\Components\RigidBodyComponent.h"
#include "..\Objects\Entity.h"
#include "..\Editor\Editor.h"
#include "..\Physics\Contact.h"
#include <cmath>

PhysicsSystem::PhysicsSystem()
{
}

PhysicsSystem::~PhysicsSystem()
{
}

void PhysicsSystem::Simulate(const float& deltaTime)
{
	for (RigidBody* rb : m_physicsEntities)
	{
		rb->ResetForce();
		rb->SetIsColliding(false);
		rb->ClearColliders();
		rb->ClearCollidingObjects();
	}

	for (RigidBody* rb : m_physicsEntities)
	{
		if (!rb->GetIsComponentEnabled())
			continue;
	
		Entity* currentEntity = rb->GetOwner();

		for (RigidBody* testAgainst : m_physicsEntities)
		{
			//if (rb->IsColliding() || testAgainst->IsColliding()) continue;
			
			// If the tags are the same that means it is the 
			// same object dont test against yourself, move on
			if (currentEntity->GetID().GetName() == testAgainst->GetOwner()->GetID().GetName())
				continue;

			if (!testAgainst->GetIsComponentEnabled())
				continue;

			if (rb->GetCollider().Intersects(&testAgainst->GetCollider()))
			{
				rb->SetIsColliding(true);
				testAgainst->SetIsColliding(true);

				rb->RecordCollidingObject(testAgainst);
				testAgainst->RecordCollidingObject(rb);

				// If it is a trigger continue, we dont want to calculate 
				// physics just record a hit
				if (rb->GetIsTrigger())
					break;

				// Have we tested against this object while going through the list before?
				// If so then just continue we dont want to test for it agian.
				// testAgainst could have been rb at one point
				if (testAgainst->IsCollidingWith(rb->GetColliderTag()))
					continue;
				else
					rb->RecordCollisionByTag(testAgainst->GetColliderTag());


				XMVECTOR direction = currentEntity->GetTransform().GetPositionVector() - testAgainst->GetOwner()->GetTransform().GetPositionVector();
				XMVECTOR directionNormalized = XMVector3Normalize(direction);
				XMFLOAT3 f_normal;
				XMStoreFloat3(&f_normal, directionNormalized);

				XMVECTOR directionLength = XMVector3Length(direction);
				XMFLOAT3 f_length;
				XMStoreFloat3(&f_length, directionLength);

				XMVECTOR hitPoint = XMVector3Normalize(directionLength) * (testAgainst->GetCollider().GetRadius());
				XMVECTOR normal = XMVector3Normalize(directionLength);
				XMFLOAT3 fn; XMStoreFloat3(&fn, normal);
				float distance = (testAgainst->GetCollider().GetRadius() + rb->GetCollider().GetRadius()) - f_length.x;
			
				m_contacts.push_back(Contact(hitPoint, directionNormalized, -distance, rb, testAgainst));
			}
		}
	}

	ProcessContacts();
}

void PhysicsSystem::AddEntity(RigidBody * rb)
{
	m_physicsEntities.push_back(rb);
}

void PhysicsSystem::ProcessContacts()
{
	using namespace DirectX;
	using namespace std;
	std::vector<Contact>::iterator contact;

	for (contact = m_contacts.begin(); contact != m_contacts.end(); contact++)
	{
		if (contact->physicsObject1->GetInverseMass() == 0.0f && contact->physicsObject2->GetInverseMass() == 0.0f) continue;

		XMVECTOR separation = contact->normal * (-contact->distance / (contact->physicsObject1->GetInverseMass() + contact->physicsObject2->GetInverseMass()));
		contact->physicsObject1->GetOwner()->GetTransform().GetPositionVectorRef() = contact->physicsObject1->GetOwner()->GetTransform().GetPositionVectorRef() + (separation * contact->physicsObject1->GetInverseMass());
		contact->physicsObject2->GetOwner()->GetTransform().GetPositionVectorRef() = contact->physicsObject2->GetOwner()->GetTransform().GetPositionVectorRef() - (separation * contact->physicsObject2->GetInverseMass());

		XMVECTOR velocity1 = XMLoadFloat3(&contact->physicsObject1->GetVelocity());
		XMVECTOR velocity2 = XMLoadFloat3(&contact->physicsObject2->GetVelocity());
		XMVECTOR relativeVelocity = velocity1 - velocity2;

		XMVECTOR normalVelocity_vec = XMVector3Dot(relativeVelocity, contact->normal);
		XMFLOAT3 vel_temp;
		XMStoreFloat3(&vel_temp, normalVelocity_vec);

		float normalVelocity = vel_temp.x;

		if (normalVelocity < 0.0f)
		{

			float restitution = min(contact->physicsObject1->GetRestitution(), contact->physicsObject2->GetRestitution());
			restitution = (abs(normalVelocity) < 0.1f) ? 0.0f : restitution;

			float j = (-(1.0f + restitution) * normalVelocity) / (contact->physicsObject1->GetInverseMass() + contact->physicsObject2->GetInverseMass());
			XMVECTOR impulse = j * contact->normal;


			XMVECTOR velocity1 = XMLoadFloat3(&contact->physicsObject1->GetVelocity());
			velocity1 += impulse * contact->physicsObject1->GetInverseMass();
			XMFLOAT3 finalVel1;
			XMStoreFloat3(&finalVel1, velocity1);
			contact->physicsObject1->SetVelocity(finalVel1);

			XMVECTOR velocity2 = XMLoadFloat3(&contact->physicsObject2->GetVelocity());
			velocity2 += impulse * contact->physicsObject2->GetInverseMass();
			XMFLOAT3 finalVel2;
			XMStoreFloat3(&finalVel2, velocity2);
			contact->physicsObject2->SetVelocity(finalVel2);

		}

	}

	m_contacts.clear();
}
