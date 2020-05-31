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

		m_pPlayerCharacter = new APlayerCharacter(0, "Player Character");

		{
			m_pTestActor = new AActor(1, "Nanosuit"); // TODO: make the id be its index in the scene
			StrongActorComponentPtr ptr = m_pTestActor->CreateDefaultSubobject<StaticMeshComponent>();
			reinterpret_cast<StaticMeshComponent*>(ptr.get())->AttachMesh(FileSystem::Get().GetRelativeAssetDirectoryPath("Models/nanosuit/nanosuit.obj"));

			m_pTestActor2 = new AActor(2, "Plane"); // TODO: make the id be its index in the scene
			StrongActorComponentPtr ptr2 = m_pTestActor2->CreateDefaultSubobject<StaticMeshComponent>();
			reinterpret_cast<StaticMeshComponent*>(ptr2.get())->AttachMesh(FileSystem::Get().GetRelativeAssetDirectoryPath("Models/Quad.obj"));
			m_pTestActor2->GetTransformRef().Scale(100.0f, 100.0f, 100.0f);

			m_pTestActor3 = new AActor(3, "Dandelion"); // TODO: make the id be its index in the scene
			StrongActorComponentPtr ptr3 = m_pTestActor3->CreateDefaultSubobject<StaticMeshComponent>();
			reinterpret_cast<StaticMeshComponent*>(ptr3.get())->AttachMesh(FileSystem::Get().GetRelativeAssetDirectoryPath("Models/Dandelion/Var1/Textured_Flower.obj"));

			m_pTestPointLight = new APointLight(4, "Point Light Actor");
			//m_pTestPointLight1 = new APointLight(4, "Point Light Actor");

		}

		m_pSceneRoot->AddChild(m_pTestActor);
		m_pSceneRoot->AddChild(m_pTestActor2);
		m_pSceneRoot->AddChild(m_pTestActor3);
		m_pSceneRoot->AddChild(m_pTestPointLight);
		//m_pSceneRoot->AddChild(m_pTestPointLight1);
		m_pSceneRoot->AddChild(m_pPlayerCharacter);


		m_Renderer->PostInit();
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
			if (m_pSelectedActor != nullptr) {
				ImGui::Text(m_pSelectedActor->GetDisplayName());
				ImGui::DragFloat3("Position", &m_pSelectedActor->GetTransformRef().GetPositionRef().x, 0.05f, -100.0f, 100.0f);
				ImGui::DragFloat3("Scale", &m_pSelectedActor->GetTransformRef().GetScaleRef().x, 0.05f, -100.0f, 100.0f);
				ImGui::DragFloat3("Rotation", &m_pSelectedActor->GetTransformRef().GetRotationRef().x, 0.05f, -100.0f, 100.0f);
			}
		}
		ImGui::End();

		/*float e = m_pPlayerCharacter->GetCameraRef().GetExposure();
		ImGui::Begin("Camera exposure");
		{
			ImGui::DragFloat("Val", &e, 0.01f, 0.0f, 1.0f);
		}
		ImGui::End();
		m_pPlayerCharacter->GetCameraRef().SetExposure(e);*/
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
		m_ModelManager.Render();
	}

	void Scene::OnMidFrameRender()
	{
		m_Renderer->OnMidFrameRender();
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
