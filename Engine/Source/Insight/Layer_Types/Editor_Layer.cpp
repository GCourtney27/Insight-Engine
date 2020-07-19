#include "ie_pch.h"

#include "Editor_Layer.h"

#include "Insight/Runtime/ACamera.h"
#include "Insight/Runtime/AActor.h"

#include "Insight/Rendering/Lighting/ASpot_Light.h"
#include "Insight/Rendering/Lighting/APoint_Light.h"
#include "Insight/Rendering/Lighting/ADirectional_Light.h"

#include "Insight/Core/Application.h"
#include "Insight/Core/Scene/Scene_Node.h"
#include "Insight/Core/Scene/Scene.h"

#include "Insight/Input/Input.h"
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
		m_pCurrentSceneRef = Application::Get().GetGameLayer().GetScene();
		m_pSceneRootRef = &m_pCurrentSceneRef->GetSceneRoot();
		m_pSceneCameraRef = &m_pCurrentSceneRef->GetSceneCamera();
	}

	void EditorLayer::OnDetach()
	{
		m_pCurrentSceneRef = nullptr;
		m_pSceneRootRef = nullptr;
		m_pSceneCameraRef = nullptr;
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
			if (ImGui::CollapsingHeader(m_pSceneRootRef->GetDisplayName(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				m_pSceneRootRef->RenderSceneHeirarchy();
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
		XMStoreFloat4x4(&viewMat, m_pSceneCameraRef->GetViewMatrix());
		XMStoreFloat4x4(&projMat, m_pSceneCameraRef->GetProjectionMatrix());

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
		ImGui::Begin("Creator");
		{
			if (ImGui::CollapsingHeader("Lights", ImGuiTreeNodeFlags_DefaultOpen)) {

				ImGuiTreeNodeFlags TreeFlags = ImGuiTreeNodeFlags_Leaf;

				ImGui::TreeNodeEx("Point Light", TreeFlags);
				if (ImGui::IsItemClicked()) {
					IE_CORE_INFO("Create Point light");
					static int PointLightIndex = 0;
					ActorType ActorType = "MyPointLight" + std::to_string(PointLightIndex++);
					m_pSceneRootRef->AddChild(new APointLight(5, ActorType));
				}
				ImGui::TreePop();

				ImGui::TreeNodeEx("Spot Light", TreeFlags);
				if (ImGui::IsItemClicked()) {
					IE_CORE_INFO("Create Spot light");
					static int SpotLightIndex = 0;
					ActorType ActorType = "MySpotLight" + std::to_string(SpotLightIndex++);
					m_pSceneRootRef->AddChild(new ASpotLight(5, ActorType));
				}

				ImGui::TreePop();
			}
			if (ImGui::CollapsingHeader("Actors", ImGuiTreeNodeFlags_DefaultOpen)) {

			}
			//if (ImGui::Button("New Point Light", { 125, 25 })) {
			//	m_pSceneRoot->AddChild(new APointLight(5, "New cool point light"));
			//}
		}
		ImGui::End();
	}

	void EditorLayer::OnUpdate(const float& DeltaMs)
	{
	}

	void EditorLayer::OnEvent(Event& event)
	{
	}

}

