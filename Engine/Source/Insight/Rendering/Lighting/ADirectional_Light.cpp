#include <ie_pch.h>

#include "ADirectional_Light.h"
#include "Platform/DirectX12/Direct3D12_Context.h"
#include "imgui.h"

namespace Insight {



	ADirectionalLight::ADirectionalLight(ActorId id, ActorType type)
		: AActor(id, type)
	{
		Direct3D12Context& graphicsContext = Direct3D12Context::Get();
		SceneNode::GetTransformRef().SetPosition(Vector3(-0.2f, -1.0f, -0.3f));
		m_ShaderCB.direction = SceneNode::GetTransformRef().GetPositionRef();
		m_ShaderCB.diffuse = XMFLOAT3(1.0f, 1.0f, 1.0f);
		m_ShaderCB.ambient = XMFLOAT3(1.0f, 1.0f, 1.0f);
		m_ShaderCB.strength = 1.0f;

		graphicsContext.AddDirectionalLight(this);
	}

	ADirectionalLight::~ADirectionalLight()
	{
	}

	bool ADirectionalLight::OnInit()
	{
		return true;
	}

	bool ADirectionalLight::OnPostInit()
	{
		return true;
	}

	void ADirectionalLight::OnUpdate(const float& deltaMs)
	{
		m_ShaderCB.direction = SceneNode::GetTransformRef().GetPositionRef();
	}

	void ADirectionalLight::OnPreRender(XMMATRIX parentMat)
	{
	}

	void ADirectionalLight::OnRender()
	{
	}

	void ADirectionalLight::Destroy()
	{
	}

	void ADirectionalLight::OnEvent(Event& e)
	{
	}

	void ADirectionalLight::BeginPlay()
	{
	}

	void ADirectionalLight::Tick(const float& deltaMs)
	{
	}

	void ADirectionalLight::Exit()
	{
	}

	void ADirectionalLight::OnImGuiRender()
	{
		AActor::OnImGuiRender();

		ImGui::Text("Rendering");
		ImGuiColorEditFlags colorWheelFlags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_PickerHueWheel;
		// Imgui will edit the color values in a normalized 0 to 1 space. 
		// In the shaders we transform the color values back into 0 to 255 space.
		ImGui::ColorEdit3("Diffuse", &m_ShaderCB.diffuse.x, colorWheelFlags);
		ImGui::ColorEdit3("Ambient", &m_ShaderCB.ambient.x, colorWheelFlags);
		ImGui::DragFloat("Strength", &m_ShaderCB.strength, 0.1f, 0.0f, 100.0f);
	}

}