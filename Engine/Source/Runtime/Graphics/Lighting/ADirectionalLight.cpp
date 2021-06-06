#include <Engine_pch.h>

#include "ADirectionalLight.h"

#include "GameFramework/Components/ActorComponent.h"
#include "Graphics/Renderer.h"
#include "GameFramework/Archetypes/ACamera.h"
#include "Core/Public/Engine.h"

#include "UI/UILib.h"

namespace Insight {



	ADirectionalLight::ADirectionalLight(ActorId id, GameFramework::ActorType type)
		: AActor(id, type)
	{
		Renderer::RegisterWorldDirectionalLight(this);

		m_pSceneComponent = CreateDefaultSubobject<GameFramework::SceneComponent>();
		m_pSceneComponent->SetEventCallback(IE_BIND_LOCAL_EVENT_FN(ADirectionalLight::OnEvent));
		m_pSceneComponent->SetRotation(0.0f, 1.0f, 6.0f);

		m_ShaderCB.DiffuseColor = FVector3(1.0f, 1.0f, 1.0f);
		m_ShaderCB.Direction = m_pSceneComponent->GetRotation();
		m_ShaderCB.Strength = 8.0f;
		m_ShaderCB.ShadowDarknessMultiplier = 0.6f;

		m_NearPlane = 1.0f;
		m_FarPlane = 210.0f;

		m_ViewWidth = 2048;
		m_ViewHeight = 2048;

		m_ShaderCB.NearZ = m_NearPlane;
		m_ShaderCB.FarZ = m_FarPlane;

		CreateProjectionMatrix(m_ShaderCB.Direction);
	}

	ADirectionalLight::~ADirectionalLight()
	{
		Renderer::UnRegisterWorldDirectionalLight();
	}

	bool ADirectionalLight::LoadFromJson(const rapidjson::Value* jsonDirectionalLight)
	{
		AActor::LoadFromJson(jsonDirectionalLight);

		float DiffuseR, DiffuseG, DiffuseB, Strength, ShdowDarkness;
		const rapidjson::Value& emission = (*jsonDirectionalLight)["Emission"];
		json::get_float(emission[0], "diffuseR", DiffuseR);
		json::get_float(emission[0], "diffuseG", DiffuseG);
		json::get_float(emission[0], "diffuseB", DiffuseB);
		json::get_float(emission[0], "strength", Strength);
		json::get_float(emission[0], "shadowDarkness", ShdowDarkness);

		m_ShaderCB.DiffuseColor = FVector3(DiffuseR, DiffuseG, DiffuseB);
		//m_ShaderCB.Direction = AActor::GetTransformRef().GetRotationRef();
		m_ShaderCB.Strength = Strength;
		m_ShaderCB.ShadowDarknessMultiplier = ShdowDarkness;

		m_NearPlane = 1.0f;
		m_FarPlane = 210.0f;
		
		LightCamPositionOffset = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

		m_ShaderCB.Direction = m_pSceneComponent->GetRotation();

		LightView = DirectX::XMMatrixLookAtLH(m_pSceneComponent->GetRotation(), m_pSceneComponent->GetPosition(), FVector3::Up);
		LightProj = DirectX::XMMatrixOrthographicLH(m_ViewWidth, m_ViewHeight, m_NearPlane, m_FarPlane);

		XMStoreFloat4x4(&LightViewFloat, XMMatrixTranspose(LightView));
		XMStoreFloat4x4(&LightProjFloat, XMMatrixTranspose(LightProj));

		m_ShaderCB.LightSpaceView = LightViewFloat;
		m_ShaderCB.LightSpaceProj = LightProjFloat;
		
		CreateProjectionMatrix(m_ShaderCB.Direction);

		return true;
	}

	bool ADirectionalLight::WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>* Writer)
	{
		Writer->StartObject(); // Start Write Actor
		{
			Writer->Key("Type");
			Writer->String("DirectionalLight");

			Writer->Key("DisplayName");
			Writer->String(StringHelper::WideToString(SceneNode::GetDisplayName()).c_str());

			// Directional Light Attributes
			Writer->Key("Emission");
			Writer->StartArray();
			{
				Writer->StartObject();
				Writer->Key("diffuseR");
				Writer->Double(m_ShaderCB.DiffuseColor.x);
				Writer->Key("diffuseG");
				Writer->Double(m_ShaderCB.DiffuseColor.y);
				Writer->Key("diffuseB");
				Writer->Double(m_ShaderCB.DiffuseColor.z);
				Writer->Key("strength");
				Writer->Double(m_ShaderCB.Strength);
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

	bool ADirectionalLight::OnInit()
	{
		return true;
	}

	bool ADirectionalLight::OnPostInit()
	{
		return true;
	}

	void ADirectionalLight::OnUpdate(const float DeltaMs)
	{
	}

	void ADirectionalLight::OnPreRender(FMatrix& parentMat)
	{
	}

	void ADirectionalLight::OnRender()
	{
	}

	void ADirectionalLight::Destroy()
	{
		Renderer::UnRegisterWorldDirectionalLight();
	}

	void ADirectionalLight::OnEvent(Event& e)
	{
		EventDispatcher Dispatcher(e);
		Dispatcher.Dispatch<TranslationEvent>(IE_BIND_LOCAL_EVENT_FN(ADirectionalLight::OnEventTranslation));
	}

	void ADirectionalLight::BeginPlay()
	{
	}

	void ADirectionalLight::Tick(const float DeltaMs)
	{
	}

	void ADirectionalLight::Exit()
	{
	}

	void ADirectionalLight::OnImGuiRender()
	{
		AActor::OnImGuiRender();

		UI::Spacing();
		UI::Spacing();

		UI::DragFloat3("light cam pos offset: ", &LightCamPositionOffset.x, 1.0f, 180.0f, 180.0f);
		UI::DragFloat("light cam near z: ", &m_NearPlane, 1.0f, 0.0f, 180.0f);
		UI::DragFloat("light cam far z: ", &m_FarPlane, 1.0f, 0.0f, 1000.0f);
		UI::DragFloat("light view width: ", &m_ViewWidth, 1.0f, 0.0f, 1000.0f);
		UI::DragFloat("light view height: ", &m_ViewHeight, 1.0f, 0.0f, 1000.0f);
		
		UI::Spacing();
		UI::Spacing();

		if (UI::CollapsingHeader("Emission", UI::TreeNode_DefaultOpen))
		{
			constexpr UI::ColorPickerFlags colorWheelFlags = UI::ColorPickerFlags_NoAlpha | UI::ColorPickerFlags_Uint8 | UI::ColorPickerFlags_PickerHueWheel;
			// Imgui will edit the color values in a normalized 0 to 1 space. 
			// In the shaders we transform the color values back into 0 to 255 space.
			UI::ColorPicker3("Diffuse", &m_ShaderCB.DiffuseColor.x, colorWheelFlags);
			UI::DragFloat("Strength", &m_ShaderCB.Strength, 0.01f, 0.0f, 10.0f);

			UI::Text("Shadows");
			UI::DragFloat("Shadow Darkness Multiplier: ", &m_ShaderCB.ShadowDarknessMultiplier, 0.05f, 0.0f, 1.0f);
		}

		m_ShaderCB.NearZ = m_NearPlane;
		m_ShaderCB.FarZ = m_FarPlane;
	}

	bool ADirectionalLight::OnEventTranslation(TranslationEvent& e)
	{
		m_ShaderCB.Direction = m_pSceneComponent->GetRotation();

		CreateProjectionMatrix(m_ShaderCB.Direction);
		
		return false;
	}

	void ADirectionalLight::CreateProjectionMatrix(FVector3 Direction)
	{
		DirectX::XMFLOAT3 LookAtPos(0.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR LookAtPosVec = XMLoadFloat3(&LookAtPos);
		DirectX::XMFLOAT3 Up(0.0f, 1.0f, 0.0f);
		DirectX::XMVECTOR UpVec = XMLoadFloat3(&Up);

		LightView = DirectX::XMMatrixLookAtLH(m_pSceneComponent->GetRotation(), LookAtPosVec, UpVec);
		LightProj = DirectX::XMMatrixOrthographicLH(m_ViewWidth, m_ViewHeight, m_NearPlane, m_FarPlane);

		XMStoreFloat4x4(&m_ShaderCB.LightSpaceView, LightView);
		XMStoreFloat4x4(&m_ShaderCB.LightSpaceProj, LightProj);
	}

}