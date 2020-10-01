#include <Engine_pch.h>

#include "APoint_Light.h"

#include "Retina/Runtime/Components/Scene_Component.h"
#include "Renderer/Renderer.h"
#include "imgui.h"

namespace Retina {



	APointLight::APointLight(ActorId id, Runtime::ActorType type)
		: AActor(id, type)
	{
		Renderer::RegisterPointLight(this);

		m_ShaderCB.DiffuseColor = ieVector3(1.0f, 1.0f, 1.0f);
		m_ShaderCB.Strength = 1.0f;

		// Load Components
		m_pSceneComponent = CreateDefaultSubobject<Runtime::SceneComponent>();
	}

	APointLight::~APointLight()
	{
	}

	bool APointLight::LoadFromJson(const rapidjson::Value* jsonPointLight)
	{
		AActor::LoadFromJson(jsonPointLight);

		float diffuseR, diffuseG, diffuseB, strength;
		const rapidjson::Value& emission = (*jsonPointLight)["Emission"];
		json::get_float(emission[0], "diffuseR", diffuseR);
		json::get_float(emission[0], "diffuseG", diffuseG);
		json::get_float(emission[0], "diffuseB", diffuseB);
		json::get_float(emission[0], "strength", strength);

		m_ShaderCB.DiffuseColor = XMFLOAT3(diffuseR, diffuseG, diffuseB);
		m_ShaderCB.Strength = strength;

		return true;
	}

	bool APointLight::WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>* Writer)
	{
		Writer->StartObject(); // Start Write Actor
		{
			Writer->Key("Type");
			Writer->String("PointLight");

			Writer->Key("DisplayName");
			Writer->String(SceneNode::GetDisplayName());

			Writer->Key("Subobjects");
			Writer->StartArray(); // Start Write SubObjects
			{
				for (size_t i = 0; i < m_NumComponents; ++i)
				{
					m_Components[i]->WriteToJson(*Writer);
				}
			}
			Writer->EndArray(); // End Write SubObjects
		}
		Writer->EndObject(); // End Write Actor
		return true;
	}

	bool APointLight::OnInit()
	{
		return true;
	}

	bool APointLight::OnPostInit()
	{
		return true;
	}

	void APointLight::OnUpdate(const float DeltaMs)
	{
		m_ShaderCB.Position = m_pSceneComponent->GetPosition();
	}

	void APointLight::OnRender()
	{
	}

	void APointLight::Destroy()
	{
		Renderer::UnRegisterPointLight(this);
	}

	void APointLight::OnEvent(Event& e)
	{
		EventDispatcher Dispatcher(e);
		Dispatcher.Dispatch<TranslationEvent>(RN_BIND_EVENT_FN(APointLight::OnEventTranslation));
	}

	void APointLight::BeginPlay()
	{
	}

	void APointLight::Tick(const float DeltaMs)
	{
	}

	void APointLight::Exit()
	{
	}

	void APointLight::OnImGuiRender()
	{
		AActor::OnImGuiRender();

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

	bool APointLight::OnEventTranslation(TranslationEvent& e)
	{
		m_ShaderCB.Position = m_pSceneComponent->GetPosition();
		return false;
	}

}