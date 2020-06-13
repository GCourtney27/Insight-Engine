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

	private:
		CB_PS_DirectionalLight m_ShaderCB;
	};

}
