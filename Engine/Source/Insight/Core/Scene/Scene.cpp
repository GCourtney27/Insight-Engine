#include <ie_pch.h>

#include "Scene.h"
#include "Insight/Core/Application.h"
#include "Insight/Runtime/Components/Static_Mesh_Component.h"
#include "imgui.h"

namespace Insight {


	Scene::Scene()
		: m_pSceneRoot(new SceneNode("Scene Root"))
	{

	}

	Scene::~Scene()
	{
		Destroy();
	}

	bool Scene::Init()
	{
		// TODO: Init from file
		m_Renderer = Application::Get().GetWindow().GetRenderContext();
		m_ModelManager.Init();

		{
			m_pTestActor = new AActor(0, "Test actor 1"); // TODO: make the id be its index in the scene
			StrongActorComponentPtr ptr = m_pTestActor->CreateDefaultSubobject<StaticMeshComponent>();
			reinterpret_cast<StaticMeshComponent*>(ptr.get())->AttachMesh("../Assets/Models/nanosuit/nanosuit.obj");
			
			m_pTestActor2 = new AActor(1, "Test actor 2"); // TODO: make the id be its index in the scene
			StrongActorComponentPtr ptr2 = m_pTestActor2->CreateDefaultSubobject<StaticMeshComponent>();
			reinterpret_cast<StaticMeshComponent*>(ptr2.get())->AttachMesh("../Assets/Objects/Primatives/sphere.obj");

			//m_pTestActor3 = new AActor(2, "Test actor 3"); // TODO: make the id be its index in the scene
			//StrongActorComponentPtr ptr3 = m_pTestActor3->CreateDefaultSubobject<StaticMeshComponent>();
			//reinterpret_cast<StaticMeshComponent*>(ptr3.get())->AttachMesh("../Assets/Models/Dandelion/Var1/Textured_Flower.obj");
		}


		m_pSceneRoot->AddChild(m_pTestActor);
		m_pSceneRoot->AddChild(m_pTestActor2);
		//m_pSceneRoot->AddChild(m_pTestActor3);


		reinterpret_cast<Direct3D12Context*>(m_Renderer.get())->CloseCommandListAndSignalCommandQueue();// Very uber doober temp
		return true;
	}

	void Scene::OnUpdate(const float& deltaMs)
	{
		m_Renderer->OnUpdate(deltaMs);
		m_pSceneRoot->OnUpdate(deltaMs);
	}

	void Scene::OnImGuiRender()
	{
		RenderSceneHeirarchy();
		RenderInspector();
	}

	void Scene::RenderSceneHeirarchy()
	{
		ImGui::Begin("Scene Heirarchy");
		{
			if (ImGui::CollapsingHeader(m_pSceneRoot->GetDisplayName(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				m_pSceneRoot->RenderSceneHeirarchy();
			}
		}
		ImGui::End();

	}

	void Scene::RenderInspector()
	{
		// TODO: If an object is selected in the scene heirarchy graph
		//		or through object ray cast picking, display its info in this panel
		
		ImGui::Begin("Inspector");
		{
			ImGui::Text(m_pTestActor->GetDisplayName());
			ImGui::DragFloat3("Position", &m_pTestActor->GetTransformRef().GetPositionRef().x, 0.05f, -100.0f, 100.0f);
			ImGui::DragFloat3("Scale", &m_pTestActor->GetTransformRef().GetScaleRef().x, 0.05f, -100.0f, 100.0f);
			ImGui::DragFloat3("Rotation", &m_pTestActor->GetTransformRef().GetRotationRef().x, 0.05f, -100.0f, 100.0f);
		}
		ImGui::End();

		ImGui::Begin("Inspector 2");
		{
			ImGui::Text(m_pTestActor2->GetDisplayName());
			ImGui::DragFloat3("Position", &m_pTestActor2->GetTransformRef().GetPositionRef().x, 0.05f, -100.0f, 100.0f);
			ImGui::DragFloat3("Scale", &m_pTestActor2->GetTransformRef().GetScaleRef().x, 0.05f, -100.0f, 100.0f);
			ImGui::DragFloat3("Rotation", &m_pTestActor2->GetTransformRef().GetRotationRef().x, 0.05f, -100.0f, 100.0f);
		}
		ImGui::End();

		/*ImGui::Begin("Inspector 3");
		{
			ImGui::Text(m_pTestActor3->GetDisplayName());
			ImGui::DragFloat3("Position", &m_pTestActor3->GetTransformRef().GetPositionRef().x, 0.05f, -100.0f, 100.0f);
			ImGui::DragFloat3("Scale", &m_pTestActor3->GetTransformRef().GetScaleRef().x, 0.05f, -100.0f, 100.0f);
			ImGui::DragFloat3("Rotation", &m_pTestActor3->GetTransformRef().GetRotationRef().x, 0.05f, -100.0f, 100.0f);
		}
		ImGui::End();*/
	}

	void Scene::OnPreRender()
	{
		m_Renderer->OnPreFrameRender();
		m_pSceneRoot->OnPreRender(XMMatrixIdentity());
		m_ModelManager.UploadVertexDataToGPU();
	}

	void Scene::OnRender()
	{
		m_Renderer->OnRender();
		m_pSceneRoot->OnRender();
		m_ModelManager.Draw();
	}

	void Scene::OnPostRender()
	{
		m_Renderer->ExecuteDraw();
		m_Renderer->SwapBuffers();
	}

	void Scene::Destroy()
	{
		delete m_pSceneRoot;
	}


}
