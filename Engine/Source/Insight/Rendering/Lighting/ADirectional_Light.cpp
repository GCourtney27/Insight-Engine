#include <ie_pch.h>

#include "ADirectional_Light.h"

#include "Insight/Runtime/Components/Actor_Component.h"
#include "Platform/Windows/DirectX12/Direct3D12_Context.h"
#include "Insight/Runtime/ACamera.h"
#include "Insight/Core/Application.h"
#include "imgui.h"

namespace Insight {



	ADirectionalLight::ADirectionalLight(ActorId id, ActorType type)
		: AActor(id, type)
	{
		Direct3D12Context& graphicsContext = Direct3D12Context::Get();
		graphicsContext.AddDirectionalLight(this);

		AActor::GetTransformRef().SetPosition(0.0f, -1.0f, -6.0f);

		m_ShaderCB.diffuse = ieVector3(1.0f, 1.0f, 1.0f);
		m_ShaderCB.direction = SceneNode::GetTransformRef().GetPosition();
		m_ShaderCB.strength = 8.0f;

		m_NearPlane = 1.0f;
		m_FarPlane = 210.0f;
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

		m_NearPlane = 1.0f;
		m_FarPlane = 210.0f;
		
		LightCamPositionOffset = XMFLOAT3(0.0f, 0.0f, 20.0f);

		m_ShaderCB.direction = SceneNode::GetTransformRef().GetPosition();

		XMFLOAT3 LookAtPos(0.0f, 0.0f, 0.0f);
		XMVECTOR LookAtPosVec = XMLoadFloat3(&LookAtPos);

		XMFLOAT3 Up(0.0f, 1.0f, 0.0f);
		XMVECTOR UpVec = XMLoadFloat3(&Up);
		XMFLOAT3 direction = m_ShaderCB.direction;
		direction.x = -direction.x + LightCamPositionOffset.x;
		direction.y =  direction.y + LightCamPositionOffset.y;
		direction.z = -direction.z + LightCamPositionOffset.z;

		LightCamPositionVec = XMLoadFloat3(&direction);


		LightView = XMMatrixLookAtLH(LightCamPositionVec, LookAtPosVec, UpVec);

		//LightProj = XMMatrixOrthographicOffCenterLH(ViewLeft, ViewWidth, ViewBottom, ViewHeight, m_NearPlane, m_FarPlane);
		LightProj = XMMatrixOrthographicLH(ViewWidth, ViewHeight, m_NearPlane, m_FarPlane);

		XMStoreFloat4x4(&LightViewFloat, XMMatrixTranspose(LightView));
		XMStoreFloat4x4(&LightProjFloat, XMMatrixTranspose(LightProj));

		m_ShaderCB.lightSpaceView = LightViewFloat;
		m_ShaderCB.lightSpaceProj = LightProjFloat;

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
		
		XMFLOAT3 LookAtPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		XMVECTOR LookAtPosVec = XMLoadFloat3(&LookAtPos);

		XMFLOAT3 Up(0.0f, 1.0f, 0.0f);
		XMVECTOR UpVec = XMLoadFloat3(&Up);
		XMFLOAT3 direction = m_ShaderCB.direction;
		direction.x = -direction.x + LightCamPositionOffset.x;
		direction.y =  direction.y + LightCamPositionOffset.y;
		direction.z = -direction.z + LightCamPositionOffset.z;
		LightCamPositionVec = XMLoadFloat3(&direction);

		LightView = XMMatrixLookAtLH(LightCamPositionVec, LookAtPosVec, UpVec);
		LightProj = XMMatrixOrthographicLH(ViewWidth, ViewHeight, m_NearPlane, m_FarPlane);

		XMStoreFloat4x4(&LightViewFloat, XMMatrixTranspose(LightView));
		XMStoreFloat4x4(&LightProjFloat, XMMatrixTranspose(LightProj));

		m_ShaderCB.lightSpaceView = LightViewFloat;
		m_ShaderCB.lightSpaceProj = LightProjFloat;
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

		ImGui::DragFloat3("light cam pos offset: ", &LightCamPositionOffset.x, 1.0f, 180.0f, 180.0f);
		ImGui::DragFloat("light cam near z: ", &m_NearPlane, 1.0f, 0.0f, 180.0f);
		ImGui::DragFloat("light cam far z: ", &m_FarPlane, 1.0f, 0.0f, 1000.0f);
		ImGui::DragFloat("light view width: ", &ViewWidth, 1.0f, 0.0f, 1000.0f);
		ImGui::DragFloat("light view height: ", &ViewHeight, 1.0f, 0.0f, 1000.0f);
		
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