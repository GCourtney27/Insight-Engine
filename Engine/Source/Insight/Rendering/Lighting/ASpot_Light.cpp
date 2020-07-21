#include <ie_pch.h>

#include "ASpot_Light.h"

#include "Insight/Runtime/Components/Actor_Component.h"
#include "Platform/Windows/DirectX_12/Direct3D12_Context.h"
#include "imgui.h"

namespace Insight {




	ASpotLight::ASpotLight(ActorId id, ActorType type)
		: AActor(id, type)
	{
		Direct3D12Context& graphicsContext = Direct3D12Context::Get();
		graphicsContext.AddSpotLight(this);

		m_ShaderCB.diffuse = ieVector3(1.0f, 1.0f, 1.0f);
		m_ShaderCB.direction = Vector3::Down;
		m_ShaderCB.strength = 1.0f;
		m_ShaderCB.innerCutoff = cos(XMConvertToRadians(m_TempInnerCutoff));
		m_ShaderCB.outerCutoff = cos(XMConvertToRadians(m_TempOuterCutoff));
	}

	ASpotLight::~ASpotLight()
	{
	}

	bool ASpotLight::LoadFromJson(const rapidjson::Value& jsonSpotLight)
	{
		AActor::LoadFromJson(jsonSpotLight);

		const rapidjson::Value& emission = jsonSpotLight["Emission"];
		json::get_float(emission[0], "diffuseR", m_ShaderCB.diffuse.x);
		json::get_float(emission[0], "diffuseG", m_ShaderCB.diffuse.y);
		json::get_float(emission[0], "diffuseB", m_ShaderCB.diffuse.z);
		json::get_float(emission[0], "directionX", m_ShaderCB.direction.x);
		json::get_float(emission[0], "directionY", m_ShaderCB.direction.y);
		json::get_float(emission[0], "directionZ", m_ShaderCB.direction.z);
		json::get_float(emission[0], "strength", m_ShaderCB.strength);
		json::get_float(emission[0], "innerCutoff", m_TempInnerCutoff);
		json::get_float(emission[0], "outerCutoff", m_TempOuterCutoff);

		m_ShaderCB.position = SceneNode::GetTransformRef().GetPosition();
		m_ShaderCB.innerCutoff = cos(XMConvertToRadians(m_TempInnerCutoff));
		m_ShaderCB.outerCutoff = cos(XMConvertToRadians(m_TempOuterCutoff));
		
		return true;
	}

	bool ASpotLight::WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer)
	{
		Writer.StartObject(); // Start Write Actor
		{
			Writer.Key("Type");
			Writer.String("SpotLight");

			Writer.Key("DisplayName");
			Writer.String(SceneNode::GetDisplayName());

			Writer.Key("Transform");
			Writer.StartArray(); // Start Write Transform
			{
				Transform& Transform = SceneNode::GetTransformRef();
				ieVector3 Pos = Transform.GetPosition();
				ieVector3 Rot = Transform.GetRotation();
				ieVector3 Sca = Transform.GetScale();

				Writer.StartObject();
				// Position
				Writer.Key("posX");
				Writer.Double(Pos.x);
				Writer.Key("posY");
				Writer.Double(Pos.y);
				Writer.Key("posZ");
				Writer.Double(Pos.z);
				// Rotation
				Writer.Key("rotX");
				Writer.Double(Rot.x);
				Writer.Key("rotY");
				Writer.Double(Rot.y);
				Writer.Key("rotZ");
				Writer.Double(Rot.z);
				// Scale
				Writer.Key("scaX");
				Writer.Double(Sca.x);
				Writer.Key("scaY");
				Writer.Double(Sca.y);
				Writer.Key("scaZ");
				Writer.Double(Sca.z);

				Writer.EndObject();
			}
			Writer.EndArray(); // End Write Transform

			// Spot Light Attributes
			Writer.Key("Emission");
			Writer.StartArray();
			{
				Writer.StartObject();
				Writer.Key("diffuseR");
				Writer.Double(m_ShaderCB.diffuse.x);
				Writer.Key("diffuseG");
				Writer.Double(m_ShaderCB.diffuse.y);
				Writer.Key("diffuseB");
				Writer.Double(m_ShaderCB.diffuse.z);
				Writer.Key("directionX");
				Writer.Double(m_ShaderCB.direction.x);
				Writer.Key("directionY");
				Writer.Double(m_ShaderCB.direction.y);
				Writer.Key("directionZ");
				Writer.Double(m_ShaderCB.direction.z);
				Writer.Key("strength");
				Writer.Double(m_ShaderCB.strength);
				Writer.Key("innerCuttoff");
				Writer.Double(m_ShaderCB.innerCutoff);
				Writer.Key("outerCuttoff");
				Writer.Double(m_ShaderCB.outerCutoff);
				Writer.EndObject();
			}
			Writer.EndArray();

			Writer.Key("Subobjects");
			Writer.StartArray(); // Start Write SubObjects
			{
				for (size_t i = 0; i < m_NumComponents; ++i)
				{
					m_Components[i]->WriteToJson(Writer);
				}
			}
			Writer.EndArray(); // End Write SubObjects
		}
		Writer.EndObject(); // End Write Actor
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

	void ASpotLight::OnUpdate(const float& deltaMs)
	{
		m_ShaderCB.position = SceneNode::GetTransformRef().GetPosition();
	}

	void ASpotLight::OnPreRender(XMMATRIX parentMat)
	{
	}

	void ASpotLight::OnRender()
	{
	}

	void ASpotLight::Destroy()
	{
	}

	void ASpotLight::OnEvent(Event& e)
	{
	}

	void ASpotLight::BeginPlay()
	{
	}

	void ASpotLight::Tick(const float& deltaMs)
	{
	}

	void ASpotLight::Exit()
	{
	}

	void ASpotLight::OnImGuiRender()
	{
		AActor::OnImGuiRender();

		ImGui::Spacing();
		ImGui::Spacing();

		if (ImGui::CollapsingHeader("Emission", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGuiColorEditFlags colorWheelFlags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_PickerHueWheel;
			// Imgui will edit the color values in a normalized 0 to 1 space. 
			// In the shaders we transform the color values back into 0 to 255 space.
			ImGui::ColorEdit3("Diffuse", &m_ShaderCB.diffuse.x, colorWheelFlags);
			ImGui::DragFloat3("Direction", &m_ShaderCB.direction.x, 0.05f, -1.0f, 1.0f);
			ImGui::DragFloat("Inner Cut-off", &m_TempInnerCutoff, 0.1f, 0.0f, 50.0f);
			ImGui::DragFloat("Outer Cut-off", &m_TempOuterCutoff, 0.1f, 0.0f, 50.0f);
			ImGui::DragFloat("Strength", &m_ShaderCB.strength, 0.15f, 0.0f, 10.0f);
			if (m_TempInnerCutoff > m_TempOuterCutoff) {
				m_TempInnerCutoff = m_TempOuterCutoff;
			}
			m_ShaderCB.innerCutoff = cos(XMConvertToRadians(m_TempInnerCutoff));
			m_ShaderCB.outerCutoff = cos(XMConvertToRadians(m_TempOuterCutoff));
		}
	}

}