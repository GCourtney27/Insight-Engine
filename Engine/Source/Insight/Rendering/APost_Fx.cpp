#include <ie_pch.h>

#include "APost_Fx.h"

#include "imgui.h"
#include "Insight/Core/Application.h"
#include "Platform/DirectX12/Direct3D12_Context.h"

namespace Insight {




	APostFx::APostFx(ActorId id, ActorType type)
		: AActor(id, type)
	{
		Direct3D12Context& graphicsContext = Direct3D12Context::Get();

		// Vignette
		m_ShaderCB.innerRadius = 0.1f;
		m_ShaderCB.outerRadius = 1.0f;
		m_ShaderCB.opacity = 1.0f;
		m_ShaderCB.vnEnabled = TRUE;
		// Film Grain
		m_ShaderCB.fgStrength = 16.0f;
		m_ShaderCB.fgEnabled = TRUE;
		// SSR
		m_ShaderCB.depthBufferSize.x = (float)Application::Get().GetWindow().GetWidth();
		m_ShaderCB.depthBufferSize.y = (float)Application::Get().GetWindow().GetHeight();
		m_ShaderCB.viewRay = APlayerCharacter::Get().GetCameraRef().GetTransformRef().GetLocalForward();
		m_ShaderCB.texelWidth = 1.0f / (float)Application::Get().GetWindow().GetWidth();
		m_ShaderCB.texelHeight = 1.0f / (float)Application::Get().GetWindow().GetHeight();
		m_ShaderCB.zThickness = 0.0006f;
		m_ShaderCB.stride = 1.0f;
		m_ShaderCB.maxSteps = 30.0f;
		m_ShaderCB.maxDistance = 15.0f;
		m_ShaderCB.strideZCutoff = 1.0f;
		m_ShaderCB.fadeStart = 0.0f;
		m_ShaderCB.fadeEnd = 1.0f;

		graphicsContext.AddPostFxActor(this);
	}

	APostFx::~APostFx()
	{
	}

	bool APostFx::OnInit()
	{
		return true;
	}

	bool APostFx::OnPostInit()
	{
		return true;
	}

	void APostFx::OnUpdate(const float& deltaMs)
	{

	}

	void APostFx::OnPreRender(XMMATRIX parentMat)
	{
	}

	void APostFx::OnRender()
	{
	}

	void APostFx::Destroy()
	{
	}

	void APostFx::OnEvent(Event& e)
	{
	}

	void APostFx::BeginPlay()
	{
	}

	void APostFx::Tick(const float& deltaMs)
	{
	}

	void APostFx::Exit()
	{
	}

	void APostFx::OnImGuiRender()
	{
		AActor::OnImGuiRender();

		ImGui::Text("Vignette");
		ImGui::Spacing();
		ImGui::Checkbox("vnEnabled", (bool*)&m_ShaderCB.vnEnabled);
		ImGui::DragFloat("Inner Radius", &m_TempInnerRadius, 0.1f, 0.0f, 50.0f);
		ImGui::DragFloat("Outer Radius", &m_TempOuterRadius, 0.1f, 0.0f, 50.0f);
		ImGui::DragFloat("Opacity", &m_ShaderCB.opacity, 0.15f, 0.0f, 10.0f);
		if (m_TempInnerRadius > m_TempOuterRadius) {
			m_TempInnerRadius = m_TempOuterRadius;
		}
		m_ShaderCB.innerRadius = m_TempInnerRadius;
		m_ShaderCB.outerRadius = m_TempOuterRadius;
		ImGui::Spacing();

		ImGui::Text("Film Grain");
		ImGui::Checkbox("fgEnabled", (bool*)&m_ShaderCB.fgEnabled);
		ImGui::DragFloat("Strength", &m_ShaderCB.fgStrength, 0.1f, 0.0f, 80.0f);
		ImGui::Spacing();

		ImGui::Text("Screen-Space Ray-Trace");
		ImGui::DragFloat("Z Thickness", &m_ShaderCB.zThickness, 0.1f, -80.0f, 80.0f);
		ImGui::DragFloat("Stride", &m_ShaderCB.stride, 0.1f, -80.0f, 80.0f);
		ImGui::DragFloat("Max Steps", &m_ShaderCB.maxSteps, 0.1f, -80.0f, 80.0f);
		ImGui::DragFloat("Max Distance", &m_ShaderCB.maxDistance, 0.1f, -80.0f, 80.0f);
		ImGui::DragFloat("Stride Z Cutoff", &m_ShaderCB.strideZCutoff, 0.1f, -80.0f, 80.0f);
		ImGui::DragFloat("Fade Start", &m_ShaderCB.fadeStart, 0.1f, -80.0f, 80.0f);
		ImGui::DragFloat("Fade End", &m_ShaderCB.fadeEnd, 0.1f, -80.0f, 80.0f);
		ImGui::DragFloat("Texel Width", &m_ShaderCB.texelWidth, 0.1f, -80.0f, 80.0f);
		ImGui::DragFloat("Texel sHeight", &m_ShaderCB.texelHeight, 0.1f, -80.0f, 80.0f);

		m_ShaderCB.viewRay = APlayerCharacter::Get().GetCameraRef().GetTransformRef().GetLocalForward();
	}

}