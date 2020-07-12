#pragma once

#include <Insight/Core.h>

#include <Insight/Runtime/AActor.h>
#include "Platform/DirectX_Shared/Constant_Buffer_Types.h"

namespace Insight {

	class INSIGHT_API ADirectionalLight : public AActor
	{
	public:
		ADirectionalLight(ActorId id, ActorType type = "Directional Light Actor");
		virtual ~ADirectionalLight();

		virtual bool LoadFromJson(const rapidjson::Value& jsonDirectionalLight) override;
		bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer) override;

		virtual bool OnInit();
		virtual bool OnPostInit();
		virtual void OnUpdate(const float& deltaMs);
		virtual void OnPreRender(XMMATRIX parentMat);
		virtual void OnRender();
		virtual void Destroy();

		void OnEvent(Event& e);

		virtual void BeginPlay();
		virtual void Tick(const float& deltaMs);
		virtual void Exit();

		virtual void OnImGuiRender() override;

		CB_PS_DirectionalLight GetConstantBuffer() { return m_ShaderCB; }

		XMFLOAT4X4 LightViewFloat;
		XMFLOAT4X4 LightProjFloat;
		XMFLOAT4 LightCamPos;
	private:
		CB_PS_DirectionalLight m_ShaderCB;
		XMVECTOR LightCamPositionVec;
		XMFLOAT4 LightCamPosition;
		float ViewWidth = 40.0f;
		float ViewLeft = -40.0f;
		float ViewHeight = 40.0f;
		float ViewBottom = -40.0f;
		XMMATRIX LightView;
		XMMATRIX LightProj;
	};

}
