#include "ie_pch.h"

#include "Editor_Layer.h"

#include "Insight/Core/Application.h"
#include "Insight/Core/Scene/Scene_Node.h"
#include "Insight/Core/Scene/Scene.h"
#include "Insight/Input/Input.h"
#include "Insight/Runtime/ACamera.h"
#include "Insight/Runtime/AActor.h"

#include "imgui.h"
#include "ImGuizmo.h"

namespace Insight {



	EditorLayer::EditorLayer()
	{
		
	}

	EditorLayer::~EditorLayer()
	{
	}

	void EditorLayer::OnAttach()
	{
		m_pCurrentScene = Application::Get().GetGameLayer().GetScene();
		m_pSceneRoot = &m_pCurrentScene->GetSceneRoot();
		m_pSceneCamera = &m_pCurrentScene->GetSceneCamera();
	}

	void EditorLayer::OnDetach()
	{
		m_pCurrentScene = nullptr;
		m_pSceneRoot = nullptr;
		m_pSceneCamera = nullptr;
	}

	void EditorLayer::OnImGuiRender()
	{
		if (!m_UIEnabled) {
			return;
		}
		RenderSceneHeirarchy();
		RenderInspector();
		RenderCreatorWindow();
	}

	void EditorLayer::RenderSceneHeirarchy()
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

	void EditorLayer::RenderInspector()
	{
		ImGui::Begin("Details");
		{
			if (m_pSelectedActor != nullptr) {

				m_pSelectedActor->OnImGuiRender();
				RenderSelectionGizmo();
			}
		}
		ImGui::End();
	}

	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
	void EditorLayer::RenderSelectionGizmo()
	{
		XMFLOAT4X4 objectMat;
		XMFLOAT4X4 deltaMat;
		XMFLOAT4X4 viewMat;
		XMFLOAT4X4 projMat;
		XMStoreFloat4x4(&objectMat, m_pSelectedActor->GetTransformRef().GetLocalMatrix());
		XMStoreFloat4x4(&viewMat, m_pSceneCamera->GetViewMatrix());
		XMStoreFloat4x4(&projMat, m_pSceneCamera->GetProjectionMatrix());

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

	void EditorLayer::RenderCreatorWindow()
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

	void EditorLayer::OnUpdate(const float& DeltaMs)
	{
	}

	void EditorLayer::OnEvent(Event& event)
	{
	}

}

