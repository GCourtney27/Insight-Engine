#include <ie_pch.h>

#include "APost_Fx.h"

#include "imgui.h"
#include "Platform/DirectX12/Direct3D12_Context.h"

namespace Insight {




	APostFx::APostFx(ActorId id, ActorType type)
		: AActor(id, type)
	{
		Direct3D12Context& graphicsContext = Direct3D12Context::Get();

		SceneNode::GetTransformRef().SetPosition(Vector3(0.0f, 60.0f, 0.0f));
		SceneNode::GetTransformRef().SetRotation(Vector3(0.0f, -1.0f, 0.0f));

		// Vignette
		m_ShaderCB.innerRadius = 0.1f;
		m_ShaderCB.outerRadius = 1.0f;
		m_ShaderCB.opacity = 1.0f;

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
		//ImGui::Checkbox("Enabled", &m_TempInnerCutoff, 0.1f, 0.0f, 50.0f);
		ImGui::DragFloat("Inner Cut-off", &m_TempInnerCutoff, 0.1f, 0.0f, 50.0f);
		ImGui::DragFloat("Outer Cut-off", &m_TempOuterCutoff, 0.1f, 0.0f, 50.0f);
		ImGui::DragFloat("Strength", &m_ShaderCB.opacity, 0.15f, 0.0f, 10.0f);
	}

}