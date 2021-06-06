#pragma once

#include "EngineDefines.h"

#include "GameFramework/AActor.h"
#include "Platform/DirectXShared/ConstantBufferTypes.h"

namespace Insight {

	class INSIGHT_API ADirectionalLight : public GameFramework::AActor
	{
	public:
		ADirectionalLight(ActorId id, GameFramework::ActorType type = TEXT("Directional Light Actor"));
		virtual ~ADirectionalLight();

		virtual bool LoadFromJson(const rapidjson::Value* jsonDirectionalLight) override;
		bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>* Writer) override;

		virtual bool OnInit();
		virtual bool OnPostInit();
		virtual void OnUpdate(const float DeltaMs);
		virtual void OnPreRender(FMatrix& parentMat);
		virtual void OnRender();
		virtual void Destroy();

		void OnEvent(Event& e);

		virtual void BeginPlay();
		virtual void Tick(const float DeltaMs);
		virtual void Exit();

		virtual void OnImGuiRender() override;

		inline CB_PS_DirectionalLight GetConstantBuffer() { return m_ShaderCB; }

		DirectX::XMFLOAT4X4 LightViewFloat;
		DirectX::XMFLOAT4X4 LightProjFloat;

	private:
		bool OnEventTranslation(TranslationEvent& e);

		void CreateProjectionMatrix(FVector3 Direction);
	private:
		CB_PS_DirectionalLight m_ShaderCB;
		GameFramework::SceneComponent* m_pSceneComponent = nullptr;

		DirectX::XMVECTOR LightCamPositionVec;
		DirectX::XMFLOAT3 LightCamPositionOffset;
		float m_NearPlane;
		float m_FarPlane;
		float m_ViewWidth;
		float m_ViewHeight;
		DirectX::XMMATRIX LightView;
		DirectX::XMMATRIX LightProj;
	};

}
