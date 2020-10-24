#include <Engine_pch.h>

#include "Scene.h"

#include "Insight/Core/Application.h"
#include "Insight/Actors/Archetypes/APlayer_Character.h"
#include "Insight/Actors/Archetypes/APlayer_Start.h"
#include "Insight/Actors/Archetypes/ACamera.h"

#include "imgui.h"

namespace Insight {


	Scene::Scene()
	{
		
	}

	Scene::~Scene()
	{
		Destroy();
	}

	bool Scene::WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>* Writer)
	{
		Writer->StartObject();
		Writer->Key("Set");
		Writer->StartArray();
		{
			m_pSceneRoot->WriteToJson(Writer);
		}
		Writer->EndArray();
		Writer->EndObject();
		return true;
	}

	bool Scene::Init(const std::string& fileName)
	{
		m_pSceneRoot = new SceneNode("Scene Root");

		// Initialize resource managers this scene will need.
		m_ResourceManager.Init();

		// Create the Scene camera and default view target. 
		// There should only be one camera in the world at 
		// any given time.
		m_EditorViewTarget = Runtime::ACamera::GetDefaultViewTarget();
		m_EditorViewTarget.FieldOfView = 75.0f;
		//m_EditorViewTarget.Position = ieVector3(-17.0f, 8.0f, -31.0f);
		m_EditorViewTarget.Position = ieVector3(83.0f, 31.0f, -23.0f);
		m_EditorViewTarget.Rotation = ieVector3(0.478f, -0.981f, 0.0f);
		m_EditorViewTarget.NearZ = 0.001f;
		m_pCamera = new Runtime::ACamera(m_EditorViewTarget);
		m_pCamera->SetCanBeFileParsed(false);
		m_pCamera->SetPerspectiveProjectionValues(
			m_EditorViewTarget.FieldOfView, 
			(float)Application::Get().GetWindow().GetWidth() / (float)Application::Get().GetWindow().GetHeight(), 
			m_EditorViewTarget.NearZ,
			m_EditorViewTarget.FarZ
		);
		m_pSceneRoot->AddChild(m_pCamera);

		// Create the player character
		m_pPlayerCharacter = new Runtime::APlayerCharacter(0);
		m_pPlayerCharacter->SetCanBeFileParsed(false);
		m_pSceneRoot->AddChild(m_pPlayerCharacter);

		// Create the player start point
		m_pPlayerStart = new Runtime::APlayerStart(0);
		m_pPlayerStart->SetCanBeFileParsed(false);
		m_pSceneRoot->AddChild(m_pPlayerStart);
		// Load the scene from .iescene folder containing all .json resource files
		FileSystem::LoadSceneFromJson(fileName, this);

		return true;
	}

	bool Scene::PostInit()
	{
		m_pSceneRoot->OnPostInit();
		return true;
	}

	void Scene::BeginPlay()
	{
		m_pCamera->SetParent(m_pPlayerCharacter);
		m_pPlayerStart->SpawnPlayer(m_pPlayerCharacter);
		m_pCamera->SetViewTarget(m_pPlayerCharacter->GetViewTarget());
		
		m_pSceneRoot->BeginPlay();
	}

	void Scene::EndPlaySession()
	{
		m_pCamera->SetParent(m_pSceneRoot);
		m_pCamera->SetViewTarget(m_EditorViewTarget);

		m_pPlayerCharacter->SetPosition(0.0f, 0.0f, 0.0f);

		m_pSceneRoot->EditorEndPlay();
	}

	void Scene::Tick(const float DeltaMs)
	{
		m_pPlayerCharacter->Tick(DeltaMs);
		m_pSceneRoot->Tick(DeltaMs);
	}

	void Scene::OnUpdate(const float DeltaMs)
	{
		m_pSceneRoot->OnUpdate(DeltaMs);
	}

	void Scene::OnImGuiRender()
	{
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
