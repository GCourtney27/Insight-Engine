#include <ie_pch.h>

#include "Scene.h"

#include "imgui.h"
#include "ImGuizmo.h"
#include "Insight/Input/Input.h"
#include "Insight/Core/Application.h"
#include "Insight/Runtime/Components/Sky_Sphere_Component.h"

namespace Insight {


	Scene::Scene()
		: m_pSceneRoot(new SceneNode("Scene Root"))
	{

	}

	Scene::~Scene()
	{
		Destroy();
	}

	bool Scene::LoadFromJson(const std::string& fileName)
	{
		FileSystem::Get().LoadSceneFromJson(fileName, this);
		return true;
	}

	bool Scene::Init(const std::string fileName)
	{
		m_Renderer = Application::Get().GetWindow().GetRenderContext();
		m_ResourceManager.Init();

		m_pPlayerCharacter = new APlayerCharacter(0, "Player Character");
		m_pSceneRoot->AddChild(m_pPlayerCharacter);

		LoadFromJson(fileName);

		m_Renderer->PostInit();
		return true;
	}

	void Scene::BeginPlay()
	{
		m_pSceneRoot->BeginPlay();
	}

	void Scene::Tick(const float& deltaMs)
	{
		IE_STRIP_FOR_GAME_DIST(if (m_TickScene)) {
			m_pSceneRoot->Tick(deltaMs);
		}
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
		RenderPlayPanel();
	}

	void Scene::EndPlaySession()
	{
		m_pSceneRoot->EditorEndPlay();
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
				XMStoreFloat4x4(&viewMat, m_pPlayerCharacter->GetCameraRef().GetViewMatrix());
				XMStoreFloat4x4(&projMat, m_pPlayerCharacter->GetCameraRef().GetProjectionMatrix());

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
						m_pSelectedActor->GetTransformRef().Rotate(rot[0] * (3.14159265f / 180.0f), rot[1] * (3.14159265f / 180.0f), rot[2] * (3.14159265f / 180.0f));
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

	void Scene::RenderPlayPanel()
	{
		ImGui::Begin("Game");
		{
			if (ImGui::Button("Play", ImVec2{ 75.0f, 50.0f })) {
				m_TickScene = true;
				BeginPlay();
			}
			if (ImGui::Button("Stop", ImVec2{ 75.0f, 50.0f })) {
				m_TickScene = false;
				EndPlaySession();
			}
		}
		ImGui::End();
	}

	void Scene::OnPreRender()
	{
		m_Renderer->OnPreFrameRender();
		m_pSceneRoot->OnPreRender(XMMatrixIdentity());
		m_ResourceManager.GetModelManager().UploadVertexDataToGPU();
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
		m_Renderer->ExecuteDraw();
		m_Renderer->SwapBuffers();
	}

	void Scene::Destroy()
	{
		delete m_pSceneRoot;
	}


}
