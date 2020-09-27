#include <Engine_pch.h>

#include "Point_Light_Component.h"

#include "Renderer/Renderer.h"
#include "Retina/Runtime/Components/Scene_Component.h"

#include "imgui.h"

namespace Retina {

	namespace Runtime {



		PointLightComponent::PointLightComponent(AActor* pOwner)
			: ActorComponent("Point Light Component", pOwner)
		{
			//Renderer::RegisterPointLight(this);

			m_ShaderCB.DiffuseColor = ieVector3(1.0f, 1.0f, 1.0f);
			m_ShaderCB.Strength = 1.0f;
		}

		PointLightComponent::~PointLightComponent()
		{
		}

		bool PointLightComponent::LoadFromJson(const rapidjson::Value& jsonStaticMeshComponent)
		{
			return false;
		}

		bool PointLightComponent::WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer)
		{
			return false;
		}

		void PointLightComponent::OnEvent(Event& e)
		{
		}

		void PointLightComponent::OnInit()
		{
		}

		void PointLightComponent::OnDestroy()
		{
		}

		void PointLightComponent::OnRender()
		{
		}

		void PointLightComponent::OnUpdate(const float& deltaTime)
		{
			m_ShaderCB.Position = m_Transform.GetPosition();
		}

		void PointLightComponent::OnImGuiRender()
		{
			ImGui::Spacing();
			ImGui::Spacing();

			if (ImGui::CollapsingHeader("Emission", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGuiColorEditFlags colorWheelFlags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_PickerHueWheel;
				// Imgui will edit the color values in a normalized 0 to 1 space. 
				// In the shaders we transform the color values back into 0 to 255 space.
				ImGui::ColorEdit3("Diffuse", &m_ShaderCB.DiffuseColor.x, colorWheelFlags);
				ImGui::DragFloat("Strength", &m_ShaderCB.Strength, 0.1f, 0.0f, 100.0f);
			}
		}

		void PointLightComponent::RenderSceneHeirarchy()
		{
		}

		void PointLightComponent::BeginPlay()
		{
		}

		void PointLightComponent::Tick(const float DeltaMs)
		{
		}

		void PointLightComponent::OnAttach()
		{
		}

		void PointLightComponent::OnDetach()
		{
		}

	} // end namespace Runtime
} // end namespace Retina
