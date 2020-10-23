#pragma once

#include <Insight/Core.h>

#include <Insight/Actors/AActor.h>
#include "Renderer/Platform/Windows/DirectX_Shared/Constant_Buffer_Types.h"

namespace Insight {

	class INSIGHT_API ADirectionalLight : public Runtime::AActor
	{
	public:
		ADirectionalLight(ActorId id, Runtime::ActorType type = "Directional Light Actor");
		virtual ~ADirectionalLight();

		virtual bool LoadFromJson(const rapidjson::Value* jsonDirectionalLight) override;
		bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>* Writer) override;

		virtual bool OnInit();
		virtual bool OnPostInit();
		virtual void OnUpdate(const float DeltaMs);
		virtual void OnPreRender(XMMATRIX parentMat);
		virtual void OnRender();
		virtual void Destroy();

		void OnEvent(Event& e);

		virtual void BeginPlay();
		virtual void Tick(const float DeltaMs);
		virtual void Exit();

		virtual void OnImGuiRender() override;

		inline CB_PS_DirectionalLight GetConstantBuffer() { return m_ShaderCB; }

		XMFLOAT4X4 LightViewFloat;
		XMFLOAT4X4 LightProjFloat;

	private:
		bool OnEventTranslation(TranslationEvent& e);

		void CreateProjectionMatrix(ieVector3 Direction);
	private:
		CB_PS_DirectionalLight m_ShaderCB;
		Runtime::SceneComponent* m_pSceneComponent = nullptr;

		XMVECTOR LightCamPositionVec;
		XMFLOAT3 LightCamPositionOffset;
		float m_NearPlane;
		float m_FarPlane;
		float m_ViewWidth;
		float m_ViewHeight;
		XMMATRIX LightView;
		XMMATRIX LightProj;
	};

}
