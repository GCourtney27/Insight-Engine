#include <ie_pch.h>

#include "ASpot_Light.h"

#include "imgui.h"
#include "Platform/DirectX12/Direct3D12_Context.h"

namespace Insight {




	ASpotLight::ASpotLight(ActorId id, ActorType type)
		: AActor(id, type)
	{
		Direct3D12Context& graphicsContext = Direct3D12Context::Get();

		SceneNode::GetTransformRef().SetPosition(Vector3(0.0f, 60.0f, 0.0f));
		SceneNode::GetTransformRef().SetRotation(Vector3(0.0f, -1.0f, 0.0f));

		m_ShaderCB.position = SceneNode::GetTransformRef().GetPositionRef();
		m_ShaderCB.direction = SceneNode::GetTransformRef().GetRotationRef();
		m_ShaderCB.diffuse = XMFLOAT3(1.0f, 1.0f, 1.0f);
		m_ShaderCB.innerCutOff = cos(XMConvertToRadians(12.5f));
		m_ShaderCB.outerCutOff = cos(XMConvertToRadians(15.0f));
		m_ShaderCB.strength = 13.0f;

		graphicsContext.AddSpotLight(this);
	}

	ASpotLight::~ASpotLight()
	{
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
		m_ShaderCB.position = SceneNode::GetTransformRef().GetPositionRef();
		m_ShaderCB.direction = SceneNode::GetTransformRef().GetRotationRef();
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

		ImGui::Text("Rendering");
		ImGuiColorEditFlags colorWheelFlags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_PickerHueWheel;
		// Imgui will edit the color values in a normalized 0 to 1 space. 
		// In the shaders we transform the color values back into 0 to 255 space.
		ImGui::ColorEdit3("Diffuse", &m_ShaderCB.diffuse.x, colorWheelFlags);
		ImGui::DragFloat("Inner Cut-off", &m_TempInnerCutoff, 0.1f, 0.0f, 50.0f);
		ImGui::DragFloat("Outer Cut-off", &m_TempOuterCutoff, 0.1f, 0.0f, 50.0f);
		ImGui::DragFloat("Strength", &m_ShaderCB.strength, 0.15f, 0.0f, 100.0f);
		if (m_TempInnerCutoff > m_TempOuterCutoff) {
			m_TempInnerCutoff = m_TempOuterCutoff;
		}
		m_ShaderCB.innerCutOff = cos(XMConvertToRadians(m_TempInnerCutoff));
		m_ShaderCB.outerCutOff = cos(XMConvertToRadians(m_TempOuterCutoff));
	}

}