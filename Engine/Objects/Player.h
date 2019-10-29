#pragma once
#include "Camera.h"
#include "..\Components\EditorSelectionComponent.h"
#include "..\Components\MeshRenderComponent.h"
#include "..\Components\LuaScriptComponent.h"

class Player : public Entity
{
public:
	Player(Scene* scene, const ID& id)
		: Entity(scene, id)
	{
		this->m_id = id;
		this->m_scene = scene;

		this->GetTransform().SetPosition(0.0f, 0.0f, -20.0f);
		this->GetTransform().SetRotation(0.0f, 0.0f, 0.0f);
		this->GetTransform().SetScale(0.2f, 0.2f, 0.2f);

		InitializeCamera(scene);
	}

	void InitializeCamera(Scene* scene);

	Camera* GetPlayerCamera() { return m_pCamera; }

private:
	Camera* m_pCamera = nullptr;
	Material* m_pMaterial = nullptr;
};

// PUT THIS AS THE PLAYER OBJECT IN scene_json.txt
/*
{
			"Type": "Player",
			"Name": "MainPlayer",
			"Transform": [
				{
					"Position": [
						{
							"x": 0.100000381469728
						},
						{
							"y": 0.800000190734864
						},
						{
							"z": 0.899999618530275
						}
					]
				},
				{
					"Rotation": [
						{
							"x": 0.0
						},
						{
							"y": 0.800000190734864
						},
						{
							"z": 0.20000000298023225
						}
					]
				},
				{
					"Scale": [
						{
							"x": 1.0
						},
						{
							"y": 1.0
						},
						{
							"z": 1.0
						}
					]
				}
			],
			"Components": [
				{
					"MeshRenderer": [
						{
							"Model": "..\\Assets\\Objects\\Dandelion\\Var1\\Textured_Flower.obj"
						}
					]
				},
				{
					"LuaScript": [
						{
							"FilePath": "..\\Assets\\LuaScripts\\PlayerController.lua"
						}
					]
				},
				{
					"EditorSelection": [
						{
							"Mode": "DEFAULT"
						}
					]
				}
			]
		},
*/