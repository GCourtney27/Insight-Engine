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

	}

}