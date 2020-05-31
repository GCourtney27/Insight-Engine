#include <ie_pch.h>

#include "APoint_Light.h"
#include "Platform/DirectX12/Direct3D12_Context.h"
#include "imgui.h"

namespace Insight {



	APointLight::APointLight(ActorId id, ActorType type)
		: AActor(id, type)
	{
		Direct3D12Context& graphicsContext = Direct3D12Context::Get();

		srand(13);
		float rColor = ((rand() % 100) / 200.0f) + 0.2f; // between 0.2 and 1.0
		float gColor = ((rand() % 100) / 200.0f) + 0.2f; // between 0.2 and 1.0
		float bColor = ((rand() % 100) / 200.0f) + 0.2f; // between 0.2 and 1.0
		m_ShaderCB.position = SceneNode::GetTransformRef().GetPositionRef();
		m_ShaderCB.ambient = XMFLOAT3(0.3f, 0.3f, 0.3f);
		m_ShaderCB.diffuse = XMFLOAT3(rColor, gColor, bColor);
		m_ShaderCB.specular = XMFLOAT3(rColor, gColor, bColor);
		m_ShaderCB.constant = 1.0f;
		m_ShaderCB.linear = 0.09f;
		m_ShaderCB.quadratic = 1.032f;

		graphicsContext.AddPointLight(this);
	}

	APointLight::~APointLight()
	{
	}

	bool APointLight::OnInit()
	{
		return false;
	}

	bool APointLight::OnPostInit()
	{
		return false;
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

		ImGui::Text("Fall-off");
		ImGui::DragFloat("Linear", &m_ShaderCB.linear, 0.01f, -1.0f, 1.0f);
		ImGui::DragFloat("Quadratic", &m_ShaderCB.quadratic, 0.01f, -1.0f, 1.0f);
		ImGui::DragFloat("Constant", &m_ShaderCB.constant, 0.01f, -1.0f, 1.0f);
		
		ImGui::Text("Rendering");
		ImGui::ColorEdit3("Diffuse", &m_ShaderCB.diffuse.x, ImGuiColorEditFlags_PickerHueWheel);
		ImGui::ColorEdit3("Specular", &m_ShaderCB.specular.x, ImGuiColorEditFlags_PickerHueWheel);
	}

}