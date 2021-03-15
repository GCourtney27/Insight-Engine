#include <Engine_pch.h>

#include "SceneComponent.h"

#include "Runtime/GameFramework/Archetypes/ACamera.h"

#include "Runtime/UI/UILib.h"
#include "Runtime/Core/Application.h"

namespace Insight {

	namespace GameFramework {

		
		SceneComponent::SceneComponent(AActor* pOwner)
			: ActorComponent("SceneComponent", pOwner)
		{
		}

		SceneComponent::~SceneComponent()
		{
		}

		bool SceneComponent::LoadFromJson(const rapidjson::Value& JsonComponent)
		{
			// Load Transform
			float X, Y, Z;
			const rapidjson::Value& Transform = JsonComponent[0]["Transform"];
			// Position
			json::get_float(Transform[0], "posX", X);
			json::get_float(Transform[0], "posY", Y);
			json::get_float(Transform[0], "posZ", Z);
			m_Transform.SetPosition(X, Y, Z);
			// Rotation
			json::get_float(Transform[0], "rotX", X);
			json::get_float(Transform[0], "rotY", Y);
			json::get_float(Transform[0], "rotZ", Z);
			m_Transform.SetRotation(X, Y, Z);
			// Scale
			json::get_float(Transform[0], "scaX", X);
			json::get_float(Transform[0], "scaZ", Z);
			json::get_float(Transform[0], "scaY", Y);
			m_Transform.SetScale(X, Y, Z);

			return true;
		}

		bool SceneComponent::WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer)
		{
			FVector Pos = m_Transform.GetPosition();
			FVector Rot = m_Transform.GetRotation();
			FVector Sca = m_Transform.GetScale();

			Writer.Key("SceneComponent");
			Writer.StartArray();
			{
				Writer.StartObject();
				{
					// Position
					Writer.Key("posX");
					Writer.Double(Pos.X());
					Writer.Key("posY");
					Writer.Double(Pos.Y());
					Writer.Key("posZ");
					Writer.Double(Pos.Z());
					// Rotation
					Writer.Key("rotX");
					Writer.Double(Rot.X());
					Writer.Key("rotY");
					Writer.Double(Rot.Y());
					Writer.Key("rotZ");
					Writer.Double(Rot.Z());
					// Scale
					Writer.Key("scaX");
					Writer.Double(Sca.X());
					Writer.Key("scaY");
					Writer.Double(Sca.Y());
					Writer.Key("scaZ");
					Writer.Double(Sca.Z());
				}
				Writer.EndObject();
			}
			Writer.EndArray();

			return true;
		}

		void SceneComponent::SetEventCallback(const EventCallbackFn& Callback)
		{
			m_TranslationData.EventCallback = Callback;
		}

		void SceneComponent::OnEvent(Event& e)
		{
		}

		void SceneComponent::OnInit()
		{
		}

		void SceneComponent::OnPostInit()
		{
			TranslationEvent e;
			e.TranslationInfo.WorldMat = m_Transform.GetLocalMatrix();
			m_TranslationData.EventCallback(e);
		}

		void SceneComponent::OnDestroy()
		{
		}

		void SceneComponent::OnRender()
		{
		}

		void SceneComponent::OnUpdate(const float& DeltaTime)
		{
		}

		void SceneComponent::OnImGuiRender()
		{
			RenderSelectionGizmo();
			
			if (UI::CollapsingHeader(m_ComponentName, UI::TreeNode_DefaultOpen)) {

				// Show the actor's transform values
				UI::Text("Transform");
				FVector3 Pos = m_Transform.GetPosition().ToFVector3();
				FVector3 Rot = m_Transform.GetRotation().ToFVector3();
				FVector3 Sca = m_Transform.GetScale().ToFVector3();
				UI::DrawVector3Control("Position", Pos);
				UI::DrawVector3Control("Rotation", Rot);
				UI::DrawVector3Control("Scale", Sca, 1.0f);
				m_Transform.SetPosition(Pos.X, Pos.Y, Pos.Z);
				m_Transform.SetRotation(Rot.X, Rot.Y, Rot.Z);
				m_Transform.SetScale(Sca.X, Sca.Y, Sca.Z);
			
				UI::Checkbox("IsStatic", &m_IsStatic);


				NotifyTranslationEvent();
			}
		}

		void SceneComponent::RenderSceneHeirarchy()
		{
		}

		void SceneComponent::BeginPlay()
		{
			m_EditorTransform = m_Transform;
		}

		void SceneComponent::EditorEndPlay()
		{
			m_EditorTransform = m_Transform;
		}

		void SceneComponent::Tick(const float DeltaMs)
		{
		}

		void SceneComponent::OnAttach()
		{
		}

		void SceneComponent::OnDetach()
		{
		}

		//static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
		//static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
		void SceneComponent::RenderSelectionGizmo()
		{
			/*static ACamera& s_WorldCameraRef = Application::Get().GetGameLayer().GetScene()->GetSceneCamera();

			XMFLOAT4X4 objectMat;
			XMFLOAT4X4 deltaMat;
			XMFLOAT4X4 viewMat;
			XMFLOAT4X4 projMat;
			XMStoreFloat4x4(&objectMat, m_Transform.GetLocalMatrix());
			XMStoreFloat4x4(&viewMat, s_WorldCameraRef.GetViewMatrix());
			XMStoreFloat4x4(&projMat, s_WorldCameraRef.GetProjectionMatrix());
#pragma message ("Fix selection guizmo!")*/

			//if (Input::IsKeyPressed('W')) {
			//	mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
			//}
			//else if (Input::IsKeyPressed('E')) {
			//	mCurrentGizmoOperation = ImGuizmo::ROTATE;
			//}
			//else if (Input::IsKeyPressed('R')) {
			//	mCurrentGizmoOperation = ImGuizmo::SCALE;
			//}

			//ImGuiIO& io = ImGui::GetIO();
			//ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
			////TODO if(Raycast::LastRayCast::Succeeded) than run this line if false than skip it (disbles the guizmo)
			//ImGuizmo::Manipulate(*viewMat.m, *projMat.m, mCurrentGizmoOperation, mCurrentGizmoMode, *objectMat.m, *deltaMat.m, NULL, NULL, NULL);

			//if (ImGuizmo::IsOver())
			//{
			//	float pos[3] = { 0.0f, 0.0f, 0.0f };
			//	float sca[3] = { 0.0f, 0.0f, 0.0f };
			//	float rot[3] = { 0.0f, 0.0f, 0.0f };

			//	switch (mCurrentGizmoOperation) {
			//	case ImGuizmo::TRANSLATE:
			//	{
			//		ImGuizmo::DecomposeMatrixToComponents(*deltaMat.m, pos, rot, sca);
			//		m_Transform.Translate(pos[0], pos[1], pos[2]);
			//		break;
			//	}
			//	case ImGuizmo::SCALE:
			//	{
			//		ImGuizmo::DecomposeMatrixToComponents(*objectMat.m, pos, rot, sca);
			//		m_Transform.SetScale(sca[0], sca[1], sca[2]);
			//		break;
			//	}
			//	case ImGuizmo::ROTATE:
			//	{
			//		ImGuizmo::DecomposeMatrixToComponents(*deltaMat.m, pos, rot, sca);
			//		m_Transform.Rotate(rot[0], rot[1], rot[2]);
			//		break;
			//	}
			//	default: { break; }
			//	}
			//}
		}

		void SceneComponent::NotifyTranslationEvent()
		{
			if (m_pParent)
			{
				m_Transform.SetWorldMatrix(XMMatrixMultiply(m_Transform.GetLocalMatrix(), m_pParent->GetTransformRef().GetWorldMatrix()));
			}
			else
			{
				m_Transform.SetWorldMatrix(m_Transform.GetLocalMatrix());
			}
			TranslationEvent e;
			e.TranslationInfo.WorldMat = m_Transform.GetWorldMatrix();
			m_TranslationData.EventCallback(e);
		}

	} // end namspace Runtime
}// end namaspace Insight
