#pragma once

#include <Insight/Core.h>

#include <Insight/Runtime/AActor.h>
#include "Renderer/Platform/Windows/DirectX_Shared/Constant_Buffer_Types.h"

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
		CB_PS_DirectionalLight m_ShaderCB;
		XMVECTOR LightCamPositionVec;
		XMFLOAT3 LightCamPositionOffset;
		float m_NearPlane;
		float m_FarPlane;
		float ViewWidth = 1024.0f;
		float ViewHeight = 1024.0f;
		XMMATRIX LightView;
		XMMATRIX LightProj;
	};

}
