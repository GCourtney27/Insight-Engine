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
		graphicsContext.AddPostFxActor(this);
	}

	APostFx::~APostFx()
	{
	}

	bool APostFx::LoadFromJson(const rapidjson::Value& jsonPostFx)
	{
		AActor::LoadFromJson(jsonPostFx);

		float vnInnerRadius, vnOuterRadius, vnOpacity; bool vnEnabled;
		float fgStrength; bool fgEnabled;
		float caIntensity; bool caEnabled;

		const rapidjson::Value& postFx = jsonPostFx["PostFx"];

		const rapidjson::Value& vignette = postFx[0];
		json::get_float(vignette, "vnInnerRadius", vnInnerRadius);
		json::get_float(vignette, "vnOuterRadius", vnOuterRadius);
		json::get_float(vignette, "vnOpacity", vnOpacity);
		json::get_bool(vignette, "vnEnabled", vnEnabled);

		const rapidjson::Value& filmGrain = postFx[1];
		json::get_float(filmGrain, "fgStrength", fgStrength);
		json::get_bool(filmGrain, "fgEnabled", fgEnabled);

		const rapidjson::Value& chromAb = postFx[2];
		json::get_float(chromAb, "caIntensity", caIntensity);
		json::get_bool(chromAb, "caEnabled", caEnabled);

		// Vignette
		m_ShaderCB.innerRadius = vnInnerRadius;
		m_ShaderCB.outerRadius = vnOuterRadius;
		m_ShaderCB.opacity = vnOpacity;
		m_ShaderCB.vnEnabled = (int)vnEnabled;
		// Film Grain
		m_ShaderCB.fgStrength = fgStrength;
		m_ShaderCB.fgEnabled = (int)fgEnabled;
		// Chromatic Aberration
		m_ShaderCB.caEnabled = (int)caEnabled;
		m_ShaderCB.caIntensity = caIntensity;
		return true;
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

		ImGui::Text("Chromatic Aberration");
		ImGui::Checkbox("caEnabled", (bool*)&m_ShaderCB.caEnabled);
		ImGui::DragFloat("Intensity", &m_ShaderCB.caIntensity, 0.1f, 0.0f, 80.0f);
	}

}