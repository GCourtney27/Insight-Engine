#include <Engine_pch.h>

#include "Editor_Layer.h"

#include "Insight/Runtime/ACamera.h"
#include "Insight/Runtime/AActor.h"

#include "Insight/Rendering/APost_Fx.h"
#include "Insight/Rendering/ASky_Sphere.h"
#include "Insight/Rendering/Lighting/ASpot_Light.h"
#include "Insight/Rendering/Lighting/APoint_Light.h"
#include "Insight/Rendering/Lighting/ADirectional_Light.h"

#include "Insight/Core/Application.h"
#include "Insight/Core/Scene/Scene_Node.h"
#include "Insight/Core/Scene/Scene.h"

#include "Insight/Input/Input.h"
#include "Insight/Layer_Types/ImGui_Layer.h"

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
					IE_CORE_INFO("Create Point light");
					static int PointLightIndex = 0;
					Runtime::ActorType ActorType = "MyPointLight" + std::to_string(PointLightIndex++);
					m_pSceneRootRef->AddChild(new APointLight(5, ActorType));
				}
				ImGui::TreePop();

				ImGui::TreeNodeEx("Spot Light", TreeFlags);
				if (ImGui::IsItemClicked()) {
					IE_CORE_INFO("Create Spot light");
					static int SpotLightIndex = 0;
					Runtime::ActorType ActorType = "MySpotLight" + std::to_string(SpotLightIndex++);
					m_pSceneRootRef->AddChild(new ASpotLight(5, ActorType));
				}
				ImGui::TreePop();

				ImGui::TreeNodeEx("Directional Light", TreeFlags);
				if (ImGui::IsItemClicked()) {
					IE_CORE_INFO("Create Directional light");
					static int DirectionalLightIndex = 0;
					Runtime::ActorType ActorType = "MyDirectionalLight" + std::to_string(DirectionalLightIndex++);
					m_pSceneRootRef->AddChild(new ADirectionalLight(5, ActorType));
				}
				ImGui::TreePop();
			}

			if (ImGui::CollapsingHeader("Actors", ImGuiTreeNodeFlags_DefaultOpen)) {

				ImGui::TreeNodeEx("Empty Actor", TreeFlags);
				if (ImGui::IsItemClicked()) {
					IE_CORE_INFO("Create Empty Actor");
					static int ActorIndex = 0;
					Runtime::ActorType ActorType = "MyActor" + std::to_string(ActorIndex++);
					m_pSceneRootRef->AddChild(new Runtime::AActor(5, ActorType));
				}
				ImGui::TreePop();

				/*ImGui::TreeNodeEx("TODO: Post-Process Actor", TreeFlags);
				if (ImGui::IsItemClicked()) {
					IE_CORE_INFO("Create Empty Actor");
					ActorType ActorType = "PostProcess Actor";
					m_pSceneRootRef->AddChild(new APostFx(5, ActorType));
				}
				ImGui::TreePop();*/


				/*ImGui::TreeNodeEx("Sky Sphere Actor", TreeFlags);
				if (ImGui::IsItemClicked()) {
					IE_CORE_INFO("Create Sky Sphere  Actor");
					ActorType ActorType = "Sky Sphere Actor";
					m_pSceneRootRef->AddChild(new ASkySphere(5, ActorType));
				}
				ImGui::TreePop();*/


				//ImGui::TreeNodeEx("TODO: Sky Light Actor", TreeFlags);
				//if (ImGui::IsItemClicked()) {
				//	IE_CORE_INFO("Create Empty Actor");
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
		Dispatcher.Dispatch<MouseButtonPressedEvent>(IE_BIND_EVENT_FN(EditorLayer::DispatchObjectSelectionRay));
	}

	ieVector3 EditorLayer::GetMouseDirectionVector()
	{
		auto [PosX, PosY] = Input::GetMousePosition();
		float WindowWidth = (float)Application::Get().GetWindow().GetWidth();
		float WindowHeight = (float)Application::Get().GetWindow().GetHeight();
		float CameraNearZ = m_pSceneCameraRef->GetNearZ();
		float CameraFarZ = m_pSceneCameraRef->GetFarZ();
		ieMatrix ProjMat = m_pSceneCameraRef->GetProjectionMatrix();
		ieMatrix ViewMat = m_pSceneCameraRef->GetViewMatrix();


		DirectX::XMVECTOR mouseNear = DirectX::XMVectorSet(PosX, PosY, 0.0f, 0.0f);

		DirectX::XMVECTOR mouseFar = DirectX::XMVectorSet(PosX, PosY, 1.0f, 0.0f);

		DirectX::XMVECTOR unprojectedNear = DirectX::XMVector3Unproject(mouseNear, 0, 0, WindowWidth, WindowHeight, CameraNearZ, CameraFarZ,
			ProjMat, ViewMat, DirectX::XMMatrixIdentity());

		DirectX::XMVECTOR unprojectedFar = DirectX::XMVector3Unproject(mouseFar, 0, 0, WindowWidth, WindowHeight, CameraNearZ,CameraFarZ,
			ProjMat, ViewMat, DirectX::XMMatrixIdentity());

		DirectX::XMVECTOR result = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(unprojectedFar, unprojectedNear));

		DirectX::XMFLOAT3 direction;

		DirectX::XMStoreFloat3(&direction, result);

		return direction;
	}

	bool EditorLayer::hit_sphere(const ieVector3& center, float radius, const Physics::Ray& r)
	{
		ieVector3 oc = r.Orgin() - center;

		float a = r.Direction().Dot(r.Direction());
		float b = 2.0f * oc.Dot(r.Direction());
		float c = oc.Dot(oc) - radius * radius;
		float discriminant = b * b - 4 * a * c;
		return (discriminant > 0.0f);
	}

	bool EditorLayer::DispatchObjectSelectionRay(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() != IE_MOUSEBUTTON_LEFT) return false;
		return false;

		ieVector3 CameraPosition = m_pSceneCameraRef->GetPosition();
		ieVector3 MouseDirection = GetMouseDirectionVector();
		Physics::Ray ray(CameraPosition, MouseDirection);

		if (hit_sphere(ieVector3(12.0f, 14.0f, 3.0f), 6.0f, ray))
		{
			IE_CORE_INFO("Ray hit");
		}

		return false;
	}

}

