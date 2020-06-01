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
			m_pTestPointLight->GetTransformRef().SetPosition(Vector3(0.0f, 13.75f, -6.3));

			//m_pTestPointLight1 = new APointLight(5, "Point Light Actor1");
			m_pDirectionalLight = new ADirectionalLight(5, "Directional Light Actor");

		}

		m_pSceneRoot->AddChild(m_pTestActor);
		m_pSceneRoot->AddChild(m_pTestActor2);
		m_pSceneRoot->AddChild(m_pTestActor3);
		/*m_pSceneRoot->AddChild(m_pTestPointLight);
		m_pSceneRoot->AddChild(m_pTestPointLight1);*/
		m_pSceneRoot->AddChild(m_pDirectionalLight);
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
		RenderCreatorWindow();
	}

	void Scene::RenderSceneHeirarchy()
	{
		ImGui::Begin("Heirarchy");
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
		ImGui::Begin("Details");
		{
			if (m_pSelectedActor != nullptr) {
				
				m_pSelectedActor->OnImGuiRender();
				// TODO add imguizmo to get transform guismo!
			}
		}
		ImGui::End();
	}

	void Scene::RenderCreatorWindow()
	{
		// TODO make this a colapsing header with different options
		ImGui::Begin("Creator");
		{
			if (ImGui::Button("New Point Light", { 125, 25 })) {
				m_pSceneRoot->AddChild(new APointLight(5, "New cool point light"));
			}
		}
		ImGui::End();
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
