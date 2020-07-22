#include <ie_pch.h>

#include "Scene.h"

#include "Insight/Core/Application.h"
#include "Insight/Runtime/APlayer_Character.h"
#include "Insight/Runtime/APlayer_Start.h"

#include "imgui.h"

namespace Insight {


	Scene::Scene()
	{
		
	}

	Scene::~Scene()
	{
		Destroy();
	}

	bool Scene::WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer)
	{
		Writer.StartObject();
		Writer.Key("Set");
		Writer.StartArray();
		{
			m_pSceneRoot->WriteToJson(Writer);
		}
		Writer.EndArray();
		Writer.EndObject();
		return true;
	}

	bool Scene::Init(const std::string fileName)
	{
		m_pSceneRoot = new SceneNode("Scene Root");

		// Get the render context from the main window
		//m_Renderer = RenderingContext::Get();

		// Initialize resource managers this scene will need.
		m_ResourceManager.Init();

		// Create the Scene camera and default view target. 
		// There should only be one camera in the world at 
		// any given time.
		m_EditorViewTarget = ACamera::GetDefaultViewTarget();
		m_EditorViewTarget.FieldOfView = 75.0f;
		//m_EditorViewTarget.Position = ieVector3(-17.0f, 8.0f, -31.0f);
		m_EditorViewTarget.Position = ieVector3(88.0f, 88.0f, -109.0f);
		m_EditorViewTarget.Rotation = ieVector3(0.478f, -.681f, 0.0f);
		m_EditorViewTarget.NearZ = 0.001f;
		m_pCamera = new ACamera(m_EditorViewTarget);
		m_pCamera->SetCanBeFileParsed(false);
		m_pCamera->SetPerspectiveProjectionValues(
			m_EditorViewTarget.FieldOfView, 
			(float)Application::Get().GetWindow().GetWidth() / (float)Application::Get().GetWindow().GetHeight(), 
			m_EditorViewTarget.NearZ,
			m_EditorViewTarget.FarZ
		);
		m_pSceneRoot->AddChild(m_pCamera);

		// Create the player character
		m_pPlayerCharacter = new APlayerCharacter(0);
		m_pPlayerCharacter->SetCanBeFileParsed(false);
		m_pSceneRoot->AddChild(m_pPlayerCharacter);

		// Create the player start point
		m_pPlayerStart = new APlayerStart(0);
		m_pPlayerStart->SetCanBeFileParsed(false);
		m_pSceneRoot->AddChild(m_pPlayerStart);
		// Load the scene from .iescene folder containing all .json resource files
		FileSystem::LoadSceneFromJson(fileName, this);

		// Tell the renderer to set init commands to the gpu
		Renderer::PostInit();
		return true;
	}

	bool Scene::PostInit()
	{
		return false;
	}

	void Scene::BeginPlay()
	{
		m_pCamera->SetParent(m_pPlayerCharacter);
		m_pPlayerStart->SpawnPlayer(m_pPlayerCharacter);
		m_pCamera->SetViewTarget(m_pPlayerCharacter->GetViewTarget());
		
		m_pPlayerCharacter->BeginPlay();

		m_pSceneRoot->BeginPlay();
	}

	void Scene::EndPlaySession()
	{
		m_pCamera->SetParent(m_pSceneRoot);
		m_pCamera->SetViewTarget(m_EditorViewTarget);

		m_pPlayerCharacter->GetTransformRef().SetPosition(0.0f, 0.0f, 0.0f);

		m_pSceneRoot->EditorEndPlay();
	}

	void Scene::Tick(const float& DeltaMs)
	{
		m_pPlayerCharacter->Tick(DeltaMs);
		m_pSceneRoot->Tick(DeltaMs);
	}

	void Scene::OnUpdate(const float& DeltaMs)
	{
		Renderer::OnUpdate(DeltaMs);
		m_pSceneRoot->OnUpdate(DeltaMs);
	}

	void Scene::OnImGuiRender()
	{
	}

	void Scene::OnPreRender()
	{
		Renderer::OnPreFrameRender();
		//Application::Get().GetImGuiLayer().Begin();
		m_pSceneRoot->CalculateParent(XMMatrixIdentity());
		m_ResourceManager.GetGeometryManager().GatherGeometry();
	}

	void Scene::OnRender()
	{
		Renderer::OnRender();
		m_pSceneRoot->OnRender();
	}

	void Scene::OnMidFrameRender()
	{
		Renderer::OnMidFrameRender();
	}

	void Scene::OnPostRender()
	{
		m_ResourceManager.GetGeometryManager().PostRender();
	}

	void Scene::Destroy()
	{
		delete m_pSceneRoot;
	}

	bool Scene::FlushAndOpenNewScene(const std::string& NewScene)
	{
		Destroy();
		m_ResourceManager.FlushAllResources();
		if (!Init(NewScene)) {
			IE_CORE_ERROR("Failed to flush current scene \"{0}\" and load new scene with filepath: \"{1}\"", m_DisplayName, NewScene);
			return false;
		}

		return true;
	}


}
