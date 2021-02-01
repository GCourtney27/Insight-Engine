// Copyright Insight Interactive. All Rights Reserved.
#include <Engine_pch.h>

#include "EditorOverlay.h"

#include "Runtime/GameFramework/Archetypes/ACamera.h"
#include "Runtime/GameFramework/AActor.h"

#include "Runtime/Rendering/APostFx.h"
#include "Runtime/Rendering/ASkySphere.h"
#include "Runtime/Rendering/Lighting/ASpotLight.h"
#include "Runtime/Rendering/Lighting/APointLight.h"
#include "Runtime/Rendering/Lighting/ADirectionalLight.h"

#include "Runtime/Core/Application.h"
#include "Runtime/Core/Scene/SceneNode.h"
#include "Runtime/Core/Scene/Scene.h"

#include "Runtime/Core/Layer/ImGuiOverlay.h"

#include "Runtime/UI/UILib.h"

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
		RenderRendererSettings();
	}

	void EditorLayer::RenderSceneHeirarchy()
	{
		UI::BeginWindow("Heirarchy");
		{
			if (UI::CollapsingHeader(m_pSceneRootRef->GetDisplayName(), UI::TreeNode_DefaultOpen))
			{
				m_pSceneRootRef->RenderSceneHeirarchy();
			}
		}
		UI::EndWindow();
	}

	void EditorLayer::RenderInspector()
	{
		UI::BeginWindow("Details");
		{
			if (m_pSelectedActor != nullptr) {

				m_pSelectedActor->OnImGuiRender();
			}
		}
		UI::EndWindow();
	}

	void EditorLayer::RenderCreatorWindow()
	{
		UI::BeginWindow("Creator");
		{
			UI::NodeFlags TreeFlags = UI::TreeNode_Leaf;

			if (UI::CollapsingHeader("Lights", UI::TreeNode_DefaultOpen)) {


				UI::TreeNodeEx("Point Light", TreeFlags);
				if (UI::IsItemClicked()) {
					IE_DEBUG_LOG(LogSeverity::Log, "Create Point light");
					static int PointLightIndex = 0;
					GameFramework::ActorType ActorType = "MyPointLight" + std::to_string(PointLightIndex++);
					m_pSceneRootRef->AddChild(new APointLight(5, ActorType));
				}
				UI::TreePopNode();

				UI::TreeNodeEx("Spot Light", TreeFlags);
				if (UI::IsItemClicked()) {
					IE_DEBUG_LOG(LogSeverity::Log, "Create Spot light");
					static int SpotLightIndex = 0;
					GameFramework::ActorType ActorType = "MySpotLight" + std::to_string(SpotLightIndex++);
					m_pSceneRootRef->AddChild(new ASpotLight(5, ActorType));
				}
				UI::TreePopNode();

				UI::TreeNodeEx("Directional Light", TreeFlags);
				if (UI::IsItemClicked()) {
					IE_DEBUG_LOG(LogSeverity::Log, "Create Directional light");
					static int DirectionalLightIndex = 0;
					GameFramework::ActorType ActorType = "MyDirectionalLight" + std::to_string(DirectionalLightIndex++);
					m_pSceneRootRef->AddChild(new ADirectionalLight(5, ActorType));
				}
				UI::TreePopNode();
			}

			if (UI::CollapsingHeader("Actors", UI::TreeNode_DefaultOpen)) {

				UI::TreeNodeEx("Empty Actor", TreeFlags);
				if (UI::IsItemClicked()) {
					IE_DEBUG_LOG(LogSeverity::Log, "Create Empty Actor");
					static int ActorIndex = 0;
					GameFramework::ActorType ActorType = "MyActor" + std::to_string(ActorIndex++);
					m_pSceneRootRef->AddChild(new GameFramework::AActor(5, ActorType));
				}
				UI::TreePopNode();

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
		UI::EndWindow();
	}

	void EditorLayer::RenderRendererSettings()
	{
		Renderer::GraphicsSettings& Settings = Renderer::GetGraphicsSettings();

		//UI::BeginWindow("Renderer");
		//{
		//	if (UI::TreeNodeEx("Textures", UI::TreeNode_DefaultOpen))
		//	{
		//		// Texture Filtering
		//		{
		//			UI::PushID("TexFilt");
		//			UI::Columns(2);
		//			UI::Text("Filtering Anisotropic");
		//			UI::NextColumn();
		//			constexpr char* TextureFilterLevels[] = { "1x", "2x", "4x", "8x", "16x" };
		//			static int SelectionIndex = (int)sqrtf((float)Settings.MaxAnisotropy);
		//			if (UI::ComboBox("##TexResolution", SelectionIndex, TextureFilterLevels, _countof(TextureFilterLevels)))
		//			{
		//				IE_DEBUG_LOG(LogSeverity::Log, "Texture Filtering: {0} (Raw Value: {1})", TextureFilterLevels[SelectionIndex], pow(2, SelectionIndex));
		//				Settings.MaxAnisotropy = (uint32_t)powf(2.0f, (float)SelectionIndex);
		//				CreateDeferredShadingRS();
		//				m_GeometryPass.SetRootSignature(m_pDeferredShadingPass_RS.Get());
		//				m_GeometryPass.ReloadShaders();
		//			}
		//			UI::EndColumns();
		//			UI::PopID();
		//		}

		//		// Texture Quality
		//		{
		//			UI::PushID("TexRes");
		//			UI::Columns(2);
		//			UI::Text("Quality (Mip LOD Bias)");
		//			UI::NextColumn();
		//			constexpr char* TextureQuality[] = { "High", "Medium", "Low" };
		//			static int SelectionIndex = (int)Settings.MipLodBias / 2;
		//			if (UI::ComboBox("##TexQuality", SelectionIndex, TextureQuality, _countof(TextureQuality)))
		//			{
		//				IE_DEBUG_LOG(LogSeverity::Log, "Texture Quality: {0} (Raw Value: {1})", TextureQuality[SelectionIndex], 2 * SelectionIndex);
		//				Settings.MipLodBias = 2.0f * (float)SelectionIndex;
		//				CreateDeferredShadingRS();
		//				m_GeometryPass.SetRootSignature(m_pDeferredShadingPass_RS.Get());
		//				m_GeometryPass.ReloadShaders();
		//			}
		//			UI::EndColumns();
		//			UI::PopID();
		//		}
		//		UI::TreePopNode();
		//	}

		//	if (UI::TreeNodeEx("Ray Tracing", UI::TreeNode_DefaultOpen))
		//	{
		//		// RT Shadows
		//		{
		//			UI::PushID("RTShadows");
		//			UI::Columns(2);
		//			UI::Text("Shadows Enabled: ");
		//			UI::NextColumn();
		//			static bool RTEnabled = Settings.RayTraceEnabled;
		//			if (UI::Checkbox("", &RTEnabled))
		//			{
		//				bool PrevState = !RTEnabled;
		//				if (PrevState) // Disabling ray tracing
		//				{
		//					m_RenderPassStack.PopPass(&m_RayTracedShadowPass);
		//				}
		//				else // Enable ray tracing
		//				{
		//					//m_RayTracedShadowPass.Create(this, &m_cbvsrvHeap, m_pRayTracePass_CommandList.Get(), nullptr);
		//					m_RenderPassStack.PushPass(&m_RayTracedShadowPass);
		//					//m_RayTracedShadowPass.GetRTHelper()->GenerateAccelerationStructure();
		//				}
		//				Settings.RayTraceEnabled = RTEnabled;
		//			}
		//			UI::EndColumns();
		//			UI::PopID();
		//		}
		//		UI::TreePopNode();
		//	}

		//	// Resolution Scale
		//	/*{
		//		UI::PushID("RenderRes");
		//		UI::Columns(2);

		//		UI::Text("Resolution Scale: ");
		//		UI::NextColumn();

		//		static float ResolutionScaleFactor = 100;
		//		if (UI::DragFloat("##RenderResolutionScale", &ResolutionScaleFactor, 1.0f, 1.0f, 100.0f))
		//		{
		//			uint32_t NewWidth = m_pWindowRef->GetWidth() * (ResolutionScaleFactor / 100.0f);
		//			uint32_t NewHeight = m_pWindowRef->GetHeight() * (ResolutionScaleFactor / 100.0f);

		//			IE_DEBUG_LOG(LogSeverity::Log, "Scale Factor: {0} (Width: {1} | Height: {2})", ResolutionScaleFactor, NewWidth, NewHeight);
		//		}

		//		UI::EndColumns();
		//		UI::NewLine();
		//		UI::PopID();
		//	}*/

		//}
		//UI::EndWindow();
	}

	void EditorLayer::OnUpdate(const float DeltaMs)
	{
	}

	void EditorLayer::OnEvent(Event& e)
	{
		EventDispatcher Dispatcher(e);
	}

}

