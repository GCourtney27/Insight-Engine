#include <ie_pch.h>

#include "Scene.h"

#include "Insight/Input/Input.h"
#include "Insight/Core/Application.h"
#include "Insight/Runtime/APlayer_Character.h"
#include "Insight/Runtime/APlayer_Start.h"

#include "imgui.h"
#include "ImGuizmo.h"

namespace Insight {


	Scene::Scene()
		: m_pSceneRoot(new SceneNode("Scene Root"))
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
		// Get the render context from the main window
		m_Renderer = Application::Get().GetWindow().GetRenderContext();

		// Initialize resource managers this scene will need.
		m_ResourceManager.Init();

		// Create the Scene camera and default view target. 
		//There should only be one camera in the world at 
		// any given time.
		m_EditorViewTarget = ACamera::GetDefaultViewTarget();
		m_EditorViewTarget.FieldOfView = 75.0f;
		m_pCamera = new ACamera(m_EditorViewTarget);
		m_pCamera->SetPerspectiveProjectionValues(
			m_EditorViewTarget.FieldOfView, 
			(float)Application::Get().GetWindow().GetWidth() / (float)Application::Get().GetWindow().GetHeight(), 
			m_EditorViewTarget.NearZ,
			m_EditorViewTarget.FarZ
		);
		m_pSceneRoot->AddChild(m_pCamera);

		// Create the player character
		m_pPlayerCharacter = new APlayerCharacter(0);
		//m_pSceneRoot->AddChild(m_pPlayerCharacter);

		// Create the player start point
		m_pPlayerStart = new APlayerStart(0);
		m_pSceneRoot->AddChild(m_pPlayerStart);

		// Load the scene from .iescene folder containing all .json resource files
		FileSystem::Get().LoadSceneFromJson(fileName, this);

		// Tell the renderer to set init commands to the gpu
		m_Renderer->PostInit();
		return true;
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
		m_Renderer->OnUpdate(DeltaMs);
		m_pSceneRoot->OnUpdate(DeltaMs);
	}

	void Scene::OnImGuiRender()
	{
		// TODO Mode this to a editor layer
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

	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
	void Scene::RenderInspector()
	{
		ImGui::Begin("Details");
		{
			if (m_pSelectedActor != nullptr) {

				m_pSelectedActor->OnImGuiRender();
				
				XMFLOAT4X4 objectMat;
				XMFLOAT4X4 deltaMat;
				XMFLOAT4X4 viewMat;
				XMFLOAT4X4 projMat;
				XMStoreFloat4x4(&objectMat, m_pSelectedActor->GetTransformRef().GetLocalMatrix());
				XMStoreFloat4x4(&viewMat, m_pCamera->GetViewMatrix());
				XMStoreFloat4x4(&projMat, m_pCamera->GetProjectionMatrix());

				if (Input::IsKeyPressed('W')) {
					mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
				}
				else if (Input::IsKeyPressed('E')) {
					mCurrentGizmoOperation = ImGuizmo::ROTATE;
				}
				else if (Input::IsKeyPressed('R')) {
					mCurrentGizmoOperation = ImGuizmo::SCALE;
				}

				ImGuiIO& io = ImGui::GetIO();
				ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
				//TODO if(Raycast::LastRayCast::Succeeded) than run this line if false than skip it (disbles the guizmo)
				ImGuizmo::Manipulate(*viewMat.m, *projMat.m, mCurrentGizmoOperation, mCurrentGizmoMode, *objectMat.m, *deltaMat.m, NULL, NULL, NULL);
				
				if (ImGuizmo::IsOver()) 
				{
					float pos[3] = { 0.0f, 0.0f, 0.0f };
					float sca[3] = { 0.0f, 0.0f, 0.0f };
					float rot[3] = { 0.0f, 0.0f, 0.0f };

					switch (mCurrentGizmoOperation) {
					case ImGuizmo::TRANSLATE:
					{
						ImGuizmo::DecomposeMatrixToComponents(*deltaMat.m, pos, rot, sca);
						m_pSelectedActor->GetTransformRef().Translate(pos[0], pos[1], pos[2]);
						break;
					}
					case ImGuizmo::SCALE:
					{
						ImGuizmo::DecomposeMatrixToComponents(*objectMat.m, pos, rot, sca);
						m_pSelectedActor->GetTransformRef().SetScale(sca[0], sca[1], sca[2]);
						break;
					}
					case ImGuizmo::ROTATE:
					{
						ImGuizmo::DecomposeMatrixToComponents(*deltaMat.m, pos, rot, sca); 
						m_pSelectedActor->GetTransformRef().Rotate(rot[0], rot[1], rot[2]);
						break;
					}
					default: { break; }
					}
				}
				
			}
		}
		ImGui::End();
	}

	void Scene::RenderCreatorWindow()
	{
		// TODO make this a colapsing header with different options
		//ImGui::Begin("Creator");
		{
			/*if (ImGui::Button("New Point Light", { 125, 25 })) {
				m_pSceneRoot->AddChild(new APointLight(5, "New cool point light"));
			}*/
		}
		//ImGui::End();
	}

	void Scene::OnPreRender()
	{
		m_Renderer->OnPreFrameRender();
		m_pSceneRoot->OnPreRender(XMMatrixIdentity());
		m_ResourceManager.GetModelManager().UploadConstantBufferDataToGPU();
	}

	void Scene::OnRender()
	{
		m_Renderer->OnRender();
		m_pSceneRoot->OnRender();
		m_ResourceManager.GetModelManager().Render();
	}

	void Scene::OnMidFrameRender()
	{
		m_Renderer->OnMidFrameRender();
	}

	void Scene::OnPostRender()
	{
		m_ResourceManager.GetModelManager().PostRender();
	}

	void Scene::Destroy()
	{
		delete m_pSceneRoot;
	}


}
