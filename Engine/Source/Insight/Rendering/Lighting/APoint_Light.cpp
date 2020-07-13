#include <ie_pch.h>

#include "APoint_Light.h"

#include "Insight/Runtime/Components/Actor_Component.h"
#include "Platform/Windows/DirectX12/Direct3D12_Context.h"
#include "imgui.h"

namespace Insight {



	APointLight::APointLight(ActorId id, ActorType type)
		: AActor(id, type)
	{
		Direct3D12Context& graphicsContext = Direct3D12Context::Get();
		graphicsContext.AddPointLight(this);
	}

	APointLight::~APointLight()
	{
	}

	bool APointLight::LoadFromJson(const rapidjson::Value& jsonPointLight)
	{
		AActor::LoadFromJson(jsonPointLight);

		float diffuseR, diffuseG, diffuseB, strength;
		const rapidjson::Value& emission = jsonPointLight["Emission"];
		json::get_float(emission[0], "diffuseR", diffuseR);
		json::get_float(emission[0], "diffuseG", diffuseG);
		json::get_float(emission[0], "diffuseB", diffuseB);
		json::get_float(emission[0], "strength", strength);

		m_ShaderCB.diffuse = XMFLOAT3(diffuseR, diffuseG, diffuseB);
		m_ShaderCB.strength = strength;

		return true;
	}

	bool APointLight::WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer)
	{
		Writer.StartObject(); // Start Write Actor
		{
			Writer.Key("Type");
			Writer.String("PointLight");

			Writer.Key("DisplayName");
			Writer.String(SceneNode::GetDisplayName());

			Writer.Key("Transform");
			Writer.StartArray(); // Start Write Transform
			{
				Transform& Transform = SceneNode::GetTransformRef();
				Vector3 Pos = Transform.GetPosition();
				Vector3 Rot = Transform.GetRotation();
				Vector3 Sca = Transform.GetScale();

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

			// Point Light Attributes
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
				Writer.Key("strength");
				Writer.Double(m_ShaderCB.strength);
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
		m_ShaderCB.position = SceneNode::GetTransformRef().GetPosition();
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

		ImGui::Spacing();
		ImGui::Spacing();

		if (ImGui::CollapsingHeader("Emission", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGuiColorEditFlags colorWheelFlags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_PickerHueWheel;
			// Imgui will edit the color values in a normalized 0 to 1 space. 
			// In the shaders we transform the color values back into 0 to 255 space.
			ImGui::ColorEdit3("Diffuse", &m_ShaderCB.diffuse.x, colorWheelFlags);
			ImGui::DragFloat("Strength", &m_ShaderCB.strength, 0.1f, 0.0f, 100.0f);
		}

	}

}