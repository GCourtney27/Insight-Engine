#include "ie_pch.h"

#include "Scene.h"
#include "Insight/Core/Application.h"
#include "Insight/Runtime/Components/Static_Mesh_Component.h"

namespace Insight {



	Scene::Scene()
	{
		m_pRoot = new SceneNode();
	}

	Scene::~Scene()
	{
		Destroy();
	}

	bool Scene::Init()
	{
		// TODO: Init from file
		m_Renderer = Application::Get().GetWindow().GetRenderContext();

		//m_pTestActor = new AActor(0);// TODO: make the id be its index in the scene
		//m_pTestActor->AddComponent<StaticMeshComponent>();

		//m_pRoot->AddChild()
		
		return true;
	}

	void Scene::OnUpdate(const float& deltaMs)
	{
		m_Renderer->OnUpdate(deltaMs);
		if (m_pRoot)
		{
			m_pRoot->OnUpdate(deltaMs);
		}
	}

	void Scene::OnPreRender()
	{
		m_Renderer->OnPreFrameRender();
	}

	void Scene::OnRender()
	{
		m_Renderer->OnRender();
		m_pRoot->OnRender();
	}

	void Scene::OnPostRender()
	{
		m_Renderer->ExecuteDraw();
		m_Renderer->SwapBuffers();
	}

	void Scene::Destroy()
	{
		delete m_pRoot;
		delete m_pTestActor;
	}

}
