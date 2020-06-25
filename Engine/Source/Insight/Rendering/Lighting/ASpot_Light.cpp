#include <ie_pch.h>

#include "ASpot_Light.h"

#include "imgui.h"
#include "Platform/DirectX12/Direct3D12_Context.h"

namespace Insight {




	ASpotLight::ASpotLight(ActorId id, ActorType type)
		: AActor(id, type)
	{
		Direct3D12Context& graphicsContext = Direct3D12Context::Get();
		graphicsContext.AddSpotLight(this);
	}

	ASpotLight::~ASpotLight()
	{
	}

	bool ASpotLight::LoadFromJson(const rapidjson::Value& jsonSpotLight)
	{
		AActor::LoadFromJson(jsonSpotLight);

		const rapidjson::Value& emission = jsonSpotLight["Emission"];
		json::get_float(emission[0], "diffuseR", m_ShaderCB.diffuse.x);
		json::get_float(emission[0], "diffuseG", m_ShaderCB.diffuse.y);
		json::get_float(emission[0], "diffuseB", m_ShaderCB.diffuse.z);
		json::get_float(emission[0], "strength", m_ShaderCB.strength);
		json::get_float(emission[0], "innerCutoff", m_TempInnerCutoff);
		json::get_float(emission[0], "outerCutoff", m_TempOuterCutoff);

		m_ShaderCB.position = SceneNode::GetTransformRef().GetPosition();
		m_ShaderCB.direction = SceneNode::GetTransformRef().GetRotation();
		m_ShaderCB.innerCutoff = cos(XMConvertToRadians(m_TempInnerCutoff));
		m_ShaderCB.outerCutoff = cos(XMConvertToRadians(m_TempOuterCutoff));

		return true;
	}

	bool ASpotLight::OnInit()
	{
		return true;
	}

	bool ASpotLight::OnPostInit()
	{
		return true;
	}

	void ASpotLight::OnUpdate(const float& deltaMs)
	{
		m_ShaderCB.position = SceneNode::GetTransformRef().GetPosition();
		m_ShaderCB.direction = SceneNode::GetTransformRef().GetRotation();
	}

	void ASpotLight::OnPreRender(XMMATRIX parentMat)
	{
	}

	void ASpotLight::OnRender()
	{
	}

	void ASpotLight::Destroy()
	{
	}

	void ASpotLight::OnEvent(Event& e)
	{
	}

	void ASpotLight::BeginPlay()
	{
	}

	void ASpotLight::Tick(const float& deltaMs)
	{
	}

	void ASpotLight::Exit()
	{
	}

	void ASpotLight::OnImGuiRender()
	{
		AActor::OnImGuiRender();

		if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGuiColorEditFlags colorWheelFlags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_PickerHueWheel;
			// Imgui will edit the color values in a normalized 0 to 1 space. 
			// In the shaders we transform the color values back into 0 to 255 space.
			ImGui::ColorEdit3("Diffuse", &m_ShaderCB.diffuse.x, colorWheelFlags);
			ImGui::DragFloat("Inner Cut-off", &m_TempInnerCutoff, 0.1f, 0.0f, 50.0f);
			ImGui::DragFloat("Outer Cut-off", &m_TempOuterCutoff, 0.1f, 0.0f, 50.0f);
			ImGui::DragFloat("Strength", &m_ShaderCB.strength, 0.15f, 0.0f, 10.0f);
			if (m_TempInnerCutoff > m_TempOuterCutoff) {
				m_TempInnerCutoff = m_TempOuterCutoff;
			}
			m_ShaderCB.innerCutoff = cos(XMConvertToRadians(m_TempInnerCutoff));
			m_ShaderCB.outerCutoff = cos(XMConvertToRadians(m_TempOuterCutoff));
		}
	}

}