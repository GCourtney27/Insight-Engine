#pragma once
#include "..\Components\RigidBodyComponent.h"
//#include <DirectXMath.h>
#include <SimpleMath.h>

struct Contact
{
	Contact(DirectX::XMVECTOR _point, DirectX::XMVECTOR _normal, float _distance, RigidBody* _PO1, RigidBody* _PO2)
		: hitPoint(_point), normal(_normal), distance(_distance), physicsObject1(_PO1), physicsObject2(_PO2) {}

	//DirectX::SimpleMath::Vector3 hitPoint = {};
	DirectX::XMVECTOR hitPoint = {};
	//DirectX::SimpleMath::Vector3 normal = {};
	DirectX::XMVECTOR normal = {};
	float distance = 0.0f;

	RigidBody* physicsObject1 = nullptr;
	RigidBody* physicsObject2 = nullptr;
};
