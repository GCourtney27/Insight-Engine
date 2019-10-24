#pragma once
#include "Camera.h"
//#include "..\Framework\Singleton.h"
//#include "..\Graphics\Graphics.h"
#include "..\Components\EditorSelectionComponent.h"

class Player : public Entity
{
public:
	Player(Scene* scene, const ID& id)
		: Entity(scene, id)
	{
		this->m_id = id;
		this->m_scene = scene;

		m_pCamera = new Camera(scene, (*new ID("PlayerCamera")));
		m_pCamera->SetParent(&this->GetTransform());
		m_pCamera->GetTransform().SetPosition(0.0f, 0.0f, 0.0f);
		m_pCamera->SetProjectionValues(80.0f, static_cast<float>(1600) / static_cast<float>(900), 0.1f, 1000.0f);
		EditorSelection* es = m_pCamera->AddComponent<EditorSelection>();
		es->Initialize(m_pCamera, 20.0f, m_pCamera->GetTransform().GetPosition());
		scene->AddEntity(m_pCamera);
		//MeshRenderer* mr = m_pCamera->AddComponent<MeshRenderer>();
		//mr->Initialize(this, "Assets\\Objects\\Primatives\\Sphere.fbx", Graphics::Instance()->GetDevice(), Graphics::Instance()->GetDeviceContext, Graphics::Instance()->GetDefaultVertexShader());
		//m_children.push_back(&m_pCamera->GetTransform());
	}

	Camera* GetPlayerCamera() { return m_pCamera; }

private:
	Camera* m_pCamera = nullptr;
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
							"Model": "Assets\\Objects\\Dandelion\\Var1\\Textured_Flower.obj"
						}
					]
				},
				{
					"LuaScript": [
						{
							"FilePath": "Assets\\LuaScripts\\PlayerController.lua"
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