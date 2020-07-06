#include <ie_pch.h>

#include "ADirectional_Light.h"

#include "Insight/Runtime/Components/Actor_Component.h"
#include "Platform/DirectX12/Direct3D12_Context.h"
#include "imgui.h"

namespace Insight {



	ADirectionalLight::ADirectionalLight(ActorId id, ActorType type)
		: AActor(id, type)
	{
		Direct3D12Context& graphicsContext = Direct3D12Context::Get();
		graphicsContext.AddDirectionalLight(this);
	}

	ADirectionalLight::~ADirectionalLight()
	{
	}

	bool ADirectionalLight::LoadFromJson(const rapidjson::Value& jsonDirectionalLight)
	{
		AActor::LoadFromJson(jsonDirectionalLight);

		float diffuseR, diffuseG, diffuseB, strength;
		const rapidjson::Value& emission = jsonDirectionalLight["Emission"];
		json::get_float(emission[0], "diffuseR", diffuseR);
		json::get_float(emission[0], "diffuseG", diffuseG);
		json::get_float(emission[0], "diffuseB", diffuseB);
		json::get_float(emission[0], "strength", strength);

		m_ShaderCB.diffuse = XMFLOAT3(diffuseR, diffuseG, diffuseB);
		m_ShaderCB.direction = AActor::GetTransformRef().GetRotationRef();
		m_ShaderCB.strength = strength;

		return true;
	}

	bool ADirectionalLight::WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer)
	{
		Writer.StartObject(); // Start Write Actor
		{
			Writer.Key("Type");
			Writer.String("DirectionalLight");

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

			// Directional Light Attributes
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

	bool ADirectionalLight::OnInit()
	{
		return true;
	}

	bool ADirectionalLight::OnPostInit()
	{
		return true;
	}

	void ADirectionalLight::OnUpdate(const float& deltaMs)
	{
		m_ShaderCB.direction = SceneNode::GetTransformRef().GetPosition();
	}

	void ADirectionalLight::OnPreRender(XMMATRIX parentMat)
	{
	}

	void ADirectionalLight::OnRender()
	{
	}

	void ADirectionalLight::Destroy()
	{
	}

	void ADirectionalLight::OnEvent(Event& e)
	{
	}

	void ADirectionalLight::BeginPlay()
	{
	}

	void ADirectionalLight::Tick(const float& deltaMs)
	{
	}

	void ADirectionalLight::Exit()
	{
	}

	void ADirectionalLight::OnImGuiRender()
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
			ImGui::DragFloat("Strength", &m_ShaderCB.strength, 0.01f, 0.0f, 10.0f);
		}

	}

}