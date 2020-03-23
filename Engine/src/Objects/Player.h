#pragma once
#include "Camera.h"
#include "..\Components\EditorSelectionComponent.h"
#include "..\Components\MeshRenderComponent.h"
#include "..\Components\LuaScriptComponent.h"

class Player : public Entity
{
public:
	Player() {}
	Player(Scene* scene, const ID& id)
		: Entity(scene, id)
	{
		this->m_id = id;
		this->m_scene = scene;
		
		InitializeCamera(scene);
	}

	bool Initialize(Scene* scene, const ID& id);

	void InitializeCamera(Scene* scene);
	
	Camera* GetPlayerCamera() { return m_pCamera; }

private:
	Camera* m_pCamera = nullptr;
	Material* m_pMaterial = nullptr;
};
