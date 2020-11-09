// Copyright Insight Interactive. All Rights Reserved.
#include <Engine_pch.h>

#include "Editor_Layer.h"

#include "Insight/Runtime/Archetypes/ACamera.h"
#include "Insight/Runtime/AActor.h"

#include "Insight/Rendering/APost_Fx.h"
#include "Insight/Rendering/ASky_Sphere.h"
#include "Insight/Rendering/Lighting/ASpot_Light.h"
#include "Insight/Rendering/Lighting/APoint_Light.h"
#include "Insight/Rendering/Lighting/ADirectional_Light.h"

#include "Insight/Core/Application.h"
#include "Insight/Core/Scene/Scene_Node.h"
#include "Insight/Core/Scene/Scene.h"

#include "Insight/Core/Layer/ImGui_Layer.h"

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
		m_pImGuiLayerRef = &Application::Get().GetImGuiLayer();
		m_pSceneRootRef = &m_pCurrentSceneRef->GetSceneRoot();
		m_pSceneCameraRef = &m_pCurrentSceneRef->GetSceneCamera();
	}

	void EditorLayer::OnDetach()
	{
		m_pCurrentSceneRef = nullptr;
		m_pImGuiLayerRef = nullptr;
		m_pSceneRootRef = nullptr;
		m_pSceneCameraRef = nullptr;
	}

	void EditorLayer::OnImGuiRender()
	{
		if (!m_UIEnabled) return;

		RenderSceneHeirarchy();
		RenderCreatorWindow();
		RenderInspector();
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

				//RenderSelectionGizmo();
				m_pSelectedActor->OnImGuiRender();
			}
		}
		ImGui::End();
	}

	void EditorLayer::RenderCreatorWindow()
	{
		ImGui::Begin("Creator");
		{
			ImGuiTreeNodeFlags TreeFlags = ImGuiTreeNodeFlags_Leaf;
			
			if (ImGui::CollapsingHeader("Lights", ImGuiTreeNodeFlags_DefaultOpen)) {


				ImGui::TreeNodeEx("Point Light", TreeFlags);
				if (ImGui::IsItemClicked()) {
					IE_DEBUG_LOG(LogSeverity::Log, "Create Point light");
					static int PointLightIndex = 0;
					Runtime::ActorType ActorType = "MyPointLight" + std::to_string(PointLightIndex++);
					m_pSceneRootRef->AddChild(new APointLight(5, ActorType));
				}
				ImGui::TreePop();

				ImGui::TreeNodeEx("Spot Light", TreeFlags);
				if (ImGui::IsItemClicked()) {
					IE_DEBUG_LOG(LogSeverity::Log, "Create Spot light");
					static int SpotLightIndex = 0;
					Runtime::ActorType ActorType = "MySpotLight" + std::to_string(SpotLightIndex++);
					m_pSceneRootRef->AddChild(new ASpotLight(5, ActorType));
				}
				ImGui::TreePop();

				ImGui::TreeNodeEx("Directional Light", TreeFlags);
				if (ImGui::IsItemClicked()) {
					IE_DEBUG_LOG(LogSeverity::Log, "Create Directional light");
					static int DirectionalLightIndex = 0;
					Runtime::ActorType ActorType = "MyDirectionalLight" + std::to_string(DirectionalLightIndex++);
					m_pSceneRootRef->AddChild(new ADirectionalLight(5, ActorType));
				}
				ImGui::TreePop();
			}

			if (ImGui::CollapsingHeader("Actors", ImGuiTreeNodeFlags_DefaultOpen)) {

				ImGui::TreeNodeEx("Empty Actor", TreeFlags);
				if (ImGui::IsItemClicked()) {
					IE_DEBUG_LOG(LogSeverity::Log, "Create Empty Actor");
					static int ActorIndex = 0;
					Runtime::ActorType ActorType = "MyActor" + std::to_string(ActorIndex++);
					m_pSceneRootRef->AddChild(new Runtime::AActor(5, ActorType));
				}
				ImGui::TreePop();

				/*ImGui::TreeNodeEx("TODO: Post-Process Actor", TreeFlags);
				if (ImGui::IsItemClicked()) {
					IE_DEBUG_LOG(LogSeverity::Log, "Create Empty Actor");
					ActorType ActorType = "PostProcess Actor";
					m_pSceneRootRef->AddChild(new APostFx(5, ActorType));
				}
				ImGui::TreePop();*/


				/*ImGui::TreeNodeEx("Sky Sphere Actor", TreeFlags);
				if (ImGui::IsItemClicked()) {
					IE_DEBUG_LOG(LogSeverity::Log, "Create Sky Sphere  Actor");
					ActorType ActorType = "Sky Sphere Actor";
					m_pSceneRootRef->AddChild(new ASkySphere(5, ActorType));
				}
				ImGui::TreePop();*/


				//ImGui::TreeNodeEx("TODO: Sky Light Actor", TreeFlags);
				//if (ImGui::IsItemClicked()) {
				//	IE_DEBUG_LOG(LogSeverity::Log, "Create Empty Actor");
				//	//static int ActorIndex = 0;
				//	//ActorType ActorType = "MyActor" + std::to_string(ActorIndex++);
				//	//m_pSceneRootRef->AddChild(new AActor(5, ActorType));
				//}
				//ImGui::TreePop();
			}

		}
		ImGui::End();
	}

	void EditorLayer::OnUpdate(const float DeltaMs)
	{
	}

	void EditorLayer::OnEvent(Event& e)
	{
		EventDispatcher Dispatcher(e);
	}

}

