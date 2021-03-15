#include <Engine_pch.h>

#include "ASpotLight.h"

#include "Runtime/GameFramework/Components/ActorComponent.h"
#include "Runtime/Graphics/Renderer.h"

#include "Runtime/UI/UILib.h"


namespace Insight {




	ASpotLight::ASpotLight(ActorId id, GameFramework::ActorType type)
		: AActor(id, type)
	{
		Renderer::RegisterSpotLight(this);

		m_ShaderCB.DiffuseColor = FVector3(1.0f, 1.0f, 1.0f);
		m_ShaderCB.Direction = FVector3::Down;
		m_ShaderCB.Strength = 1.0f;
		m_ShaderCB.InnerCutoff = cos(DEGREES_TO_RADIANS(m_TempInnerCutoff));
		m_ShaderCB.OuterCutoff = cos(DEGREES_TO_RADIANS(m_TempOuterCutoff));

		m_pSceneComponent = CreateDefaultSubobject<GameFramework::SceneComponent>();

	}

	ASpotLight::~ASpotLight()
	{
	}

	bool ASpotLight::LoadFromJson(const rapidjson::Value* jsonSpotLight)
	{
		AActor::LoadFromJson(jsonSpotLight);

		const rapidjson::Value& emission = (*jsonSpotLight)["Emission"];
		json::get_float(emission[0], "diffuseR", m_ShaderCB.DiffuseColor.X);
		json::get_float(emission[0], "diffuseG", m_ShaderCB.DiffuseColor.Y);
		json::get_float(emission[0], "diffuseB", m_ShaderCB.DiffuseColor.Z);
		json::get_float(emission[0], "directionX", m_ShaderCB.Direction.X);
		json::get_float(emission[0], "directionY", m_ShaderCB.Direction.Y);
		json::get_float(emission[0], "directionZ", m_ShaderCB.Direction.Z);
		json::get_float(emission[0], "strength", m_ShaderCB.Strength);
		json::get_float(emission[0], "innerCutoff", m_TempInnerCutoff);
		json::get_float(emission[0], "outerCutoff", m_TempOuterCutoff);

		//m_ShaderCB.Position = SceneNode::GetTransformRef().GetPosition();
		m_ShaderCB.InnerCutoff = cos(RADIANS_TO_DEGREES(m_TempInnerCutoff));
		m_ShaderCB.OuterCutoff = cos(RADIANS_TO_DEGREES(m_TempOuterCutoff));
		
		return true;
	}

	bool ASpotLight::WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>* Writer)
	{
		Writer->StartObject(); // Start Write Actor
		{
			Writer->Key("Type");
			Writer->String("SpotLight");

			Writer->Key("DisplayName");
			Writer->String(StringHelper::WideToString(SceneNode::GetDisplayName()).c_str());

			Writer->Key("Transform");
			Writer->StartArray(); // Start Write Transform
			{
				//ieTransform& Transform = SceneNode::GetTransformRef();
				//FVector3 Pos = Transform.GetPosition();
				//FVector3 Rot = Transform.GetRotation();
				//FVector3 Sca = Transform.GetScale();

				//Writer->StartObject();
				//// Position
				//Writer->Key("posX");
				//Writer->Double(Pos.X);
				//Writer->Key("posY");
				//Writer->Double(Pos.Y);
				//Writer->Key("posZ");
				//Writer->Double(Pos.Z);
				//// Rotation
				//Writer->Key("rotX");
				//Writer->Double(Rot.X);
				//Writer->Key("rotY");
				//Writer->Double(Rot.Y);
				//Writer->Key("rotZ");
				//Writer->Double(Rot.Z);
				//// Scale
				//Writer->Key("scaX");
				//Writer->Double(Sca.X);
				//Writer->Key("scaY");
				//Writer->Double(Sca.Y);
				//Writer->Key("scaZ");
				//Writer->Double(Sca.Z);

				Writer->EndObject();
			}
			Writer->EndArray(); // End Write Transform

			// Spot Light Attributes
			Writer->Key("Emission");
			Writer->StartArray();
			{
				Writer->StartObject();
				Writer->Key("diffuseR");
				Writer->Double(m_ShaderCB.DiffuseColor.X);
				Writer->Key("diffuseG");
				Writer->Double(m_ShaderCB.DiffuseColor.Y);
				Writer->Key("diffuseB");
				Writer->Double(m_ShaderCB.DiffuseColor.Z);
				Writer->Key("directionX");
				Writer->Double(m_ShaderCB.Direction.X);
				Writer->Key("directionY");
				Writer->Double(m_ShaderCB.Direction.Y);
				Writer->Key("directionZ");
				Writer->Double(m_ShaderCB.Direction.Z);
				Writer->Key("strength");
				Writer->Double(m_ShaderCB.Strength);
				Writer->Key("innerCuttoff");
				Writer->Double(m_ShaderCB.InnerCutoff);
				Writer->Key("outerCuttoff");
				Writer->Double(m_ShaderCB.OuterCutoff);
				Writer->EndObject();
			}
			Writer->EndArray();

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

	bool ASpotLight::OnInit()
	{
		return true;
	}

	bool ASpotLight::OnPostInit()
	{
		return true;
	}

	void ASpotLight::OnUpdate(const float DeltaMs)
	{
	}

	void ASpotLight::OnPreRender(ieMatrix& parentMat)
	{
	}

	void ASpotLight::OnRender()
	{
	}

	void ASpotLight::Destroy()
	{
		Renderer::UnRegisterSpotLight(this);
	}

	void ASpotLight::OnEvent(Event& e)
	{
	}

	bool ASpotLight::OnEventTranslation(TranslationEvent& e)
	{
		m_ShaderCB.Position = m_pSceneComponent->GetPosition().ToFVector3();
		return false;
	}

	void ASpotLight::BeginPlay()
	{
	}

	void ASpotLight::Tick(const float DeltaMs)
	{
	}

	void ASpotLight::Exit()
	{
	}

	void ASpotLight::OnImGuiRender()
	{
		AActor::OnImGuiRender();

		UI::Spacing();
		UI::Spacing();

		if (UI::CollapsingHeader("Emission", UI::TreeNode_DefaultOpen))
		{
			constexpr UI::ColorPickerFlags colorWheelFlags = UI::ColorPickerFlags_NoAlpha | UI::ColorPickerFlags_Uint8 | UI::ColorPickerFlags_PickerHueWheel;

			// Imgui will edit the color values in a normalized 0 to 1 space. 
			// In the shaders we transform the color values back into 0 to 255 space.
			UI::ColorPicker3("Diffuse", &m_ShaderCB.DiffuseColor.X, colorWheelFlags);
			UI::DragFloat3("Direction", &m_ShaderCB.Direction.X, 0.05f, -1.0f, 1.0f);
			UI::DragFloat("Inner Cut-off", &m_TempInnerCutoff, 0.1f, 0.0f, 50.0f);
			UI::DragFloat("Outer Cut-off", &m_TempOuterCutoff, 0.1f, 0.0f, 50.0f);
			UI::DragFloat("Strength", &m_ShaderCB.Strength, 0.15f, 0.0f, 10.0f);
			if (m_TempInnerCutoff > m_TempOuterCutoff) {
				m_TempInnerCutoff = m_TempOuterCutoff;
			}
			m_ShaderCB.InnerCutoff = cos(DEGREES_TO_RADIANS(m_TempInnerCutoff));
			m_ShaderCB.OuterCutoff = cos(DEGREES_TO_RADIANS(m_TempOuterCutoff));
		}
	}

}