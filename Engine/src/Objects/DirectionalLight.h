#pragma once
#include "Entity.h"

class DirectionalLight : public Entity
{
public: 
	DirectionalLight(Scene* pScene, const ID& id)
		: Entity(pScene, id)
	{
		this->m_id = id;
		this->m_scene = pScene;
	}
	virtual ~DirectionalLight() {}

	virtual void Destroy() override;

	// -- Runtime Methods (Components / Game Logic) -- //
	virtual void OnStart() override;
	virtual void OnUpdate(const float& deltaTime) override;
	virtual void OnExit() override;

	DirectX::XMFLOAT3 lightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float lightStrength = 1.0f;

};