#pragma once
#include "Entity.h"

class PointLight : public Entity
{
public:
	PointLight(Scene* scene, const ID& id) 
		: Entity(scene, id)
	{
		this->m_id = id;
		this->m_scene = scene;
	}
	virtual ~PointLight() {}

	virtual void Destroy() override;

	// -- Runtime Methods (Components / Game Logic) -- //
	virtual void OnStart() override;
	virtual void OnUpdate(const float& deltaTime) override;
	virtual void OnExit() override;

	DirectX::XMFLOAT3 lightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float lightStrength = 1.0f;
	float attenuation_a = 1.0f;
	float attenuation_b = 1.0f;
	float attenuation_c = 0.1f;
};