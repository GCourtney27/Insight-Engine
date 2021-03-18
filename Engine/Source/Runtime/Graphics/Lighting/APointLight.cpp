#include <Engine_pch.h>

#include "APointLight.h"

#include "Runtime/GameFramework/Components/SceneComponent.h"
#include "Runtime/Graphics/Renderer.h"
#include "Runtime/UI/UILib.h"

namespace Insight {



	APointLight::APointLight(ActorId id, GameFramework::ActorType type)
		: AActor(id, type)
	{
		Renderer::RegisterPointLight(this);

		m_ShaderCB.DiffuseColor = FVector3(1.0f, 1.0f, 1.0f);
		m_ShaderCB.Strength = 1.0f;

		// Load Components
		m_pSceneComponent = CreateDefaultSubobject<GameFramework::SceneComponent>();
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

		m_ShaderCB.DiffuseColor = FVector3(diffuseR, diffuseG, diffuseB);
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
			Writer->String(StringHelper::WideToString(SceneNode::GetDisplayName()).c_str());

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
		Dispatcher.Dispatch<TranslationEvent>(IE_BIND_LOCAL_EVENT_FN(APointLight::OnEventTranslation));
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

		UI::Spacing();
		UI::Spacing();

		if (UI::CollapsingHeader("Emission", UI::TreeNode_DefaultOpen))
		{
			constexpr UI::ColorPickerFlags colorWheelFlags = UI::ColorPickerFlags_NoAlpha | UI::ColorPickerFlags_Uint8 | UI::ColorPickerFlags_PickerHueWheel;

			// Imgui will edit the color values in a normalized 0 to 1 space. 
			// In the shaders we transform the color values back into 0 to 255 space.
			UI::ColorPicker3("Diffuse", &m_ShaderCB.DiffuseColor.x, colorWheelFlags);
			UI::DragFloat("Strength", &m_ShaderCB.Strength, 0.1f, 0.0f, 100.0f);
		}

	}

	bool APointLight::OnEventTranslation(TranslationEvent& e)
	{
		m_ShaderCB.Position = m_pSceneComponent->GetPosition();
		return false;
	}

}