#include <ie_pch.h>

#include "APoint_Light.h"
#include "Platform/DirectX12/Direct3D12_Context.h"
#include "imgui.h"

namespace Insight {



	APointLight::APointLight(ActorId id, ActorType type)
		: AActor(id, type)
	{
		Direct3D12Context& graphicsContext = Direct3D12Context::Get();

		m_ShaderCB.position = SceneNode::GetTransformRef().GetPositionRef();
		m_ShaderCB.diffuse = XMFLOAT3(255.0f, 255.0f, 255.0f);

		graphicsContext.AddPointLight(this);
	}

	APointLight::~APointLight()
	{
	}

	bool APointLight::OnInit()
	{
		return true;
	}

	bool APointLight::OnPostInit()
	{
		return true;
	}

	void APointLight::OnUpdate(const float& deltaMs)
	{
		m_ShaderCB.position = SceneNode::GetTransformRef().GetPositionRef();
	}

	void APointLight::OnPreRender(XMMATRIX parentMat)
	{
	}

	void APointLight::OnRender()
	{
	}

	void APointLight::Destroy()
	{
	}

	void APointLight::OnEvent(Event& e)
	{
	}

	void APointLight::BeginPlay()
	{
	}

	void APointLight::Tick(const float& deltaMs)
	{
	}

	void APointLight::Exit()
	{
	}

	void APointLight::OnImGuiRender()
	{
		AActor::OnImGuiRender();
		
		ImGui::Text("Rendering");
		ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_PickerHueWheel;
		//static float r = 0.0f;
		//static float g = 0.0f;
		//static float b = 0.0f;
		ImGui::DragFloat3("Diffuse", &m_ShaderCB.diffuse.x, 1.0f, 0.0f, 255.0f);
		//ImGui::ColorEdit3("Diffuse", &m_ShaderCB.diffuse.x, flags);
		
	}

}