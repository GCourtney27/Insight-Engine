#include <Engine_pch.h>

#include "Sphere_Collider.h"

#include "Insight/Events/Application_Event.h"
#include "Insight/Systems/Managers/Physics_Manager.h"

#include "imgui.h"

namespace Insight {

	namespace Runtime {


		uint32_t SphereColliderComponent::s_NumActiveSphereColliderComponents = 0U;


		SphereColliderComponent::SphereColliderComponent(AActor* pOwner)
			: ActorComponent("Sphere Collider Component", pOwner)
		{
			m_ColliderType = ColliderType::SPHERE;

		}

		SphereColliderComponent::~SphereColliderComponent()
		{
		}

		bool SphereColliderComponent::LoadFromJson(const rapidjson::Value& JsonSphereColliderComponent)
		{
			//json::get_bool(JsonSphereColliderComponent, "IsStatic", m_IsStatic);
			return true;
		}

		bool SphereColliderComponent::WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer)
		{

			return true;
		}

		void SphereColliderComponent::OnEvent(Event& e)
		{
		}

		void SphereColliderComponent::OnInit()
		{
		}

		void SphereColliderComponent::OnDestroy()
		{
		}

		void SphereColliderComponent::CalculateParent(const DirectX::XMMATRIX& Matrix)
		{
		}

		void SphereColliderComponent::OnRender()
		{
		}

		void SphereColliderComponent::OnImGuiRender()
		{
			ImGui::Spacing();
			ImGui::PushID(m_IDBuffer);

			if (ImGui::CollapsingHeader(m_ComponentName, ImGuiTreeNodeFlags_DefaultOpen)) {

				ImGui::Checkbox("Is World Static: ", &m_IsStatic);

			}

			ImGui::PopID();
		}

		void SphereColliderComponent::RenderSceneHeirarchy()
		{
		}

		void SphereColliderComponent::BeginPlay()
		{
			PhysicsEvent e;

			m_CollisionData.EventCallback(e);
		}

		void SphereColliderComponent::Tick(const float DeltaMs)
		{
		}

		void SphereColliderComponent::OnAttach()
		{
			m_SphereColliderWorldIndex = s_NumActiveSphereColliderComponents++;
			sprintf_s(m_IDBuffer, "SM-%u", m_SphereColliderWorldIndex);

			PhysicsManager::RegisterPhysicsObject(this);
		}

		void SphereColliderComponent::OnDetach()
		{
			PhysicsManager::UnRegisterPhysicsObject(this);

		}

	} // end namespace Runtime
} // end namespace Insight
