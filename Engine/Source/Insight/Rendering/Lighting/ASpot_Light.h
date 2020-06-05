#pragma once

#include <Insight/Core.h>

#include "Insight/Runtime/AActor.h"
#include "Platform/DirectX_Shared/Constant_Buffer_Types.h"

namespace Insight {

	class INSIGHT_API ASpotLight : public AActor
	{
	public:
		ASpotLight(ActorId id, ActorType type = "Spot Light Actor");
		virtual ~ASpotLight();

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

		CB_PS_SpotLight GetConstantBuffer() { return m_ShaderCB; }

	private:
		CB_PS_SpotLight m_ShaderCB;
		float m_TempInnerCutoff = 12.5f;
		float m_TempOuterCutoff = 15.0f;
	};

}