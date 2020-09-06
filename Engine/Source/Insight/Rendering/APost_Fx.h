#pragma once

#include <Insight/Core.h>

#include "Insight/Runtime/AActor.h"
#include "Renderer/Platform/Windows/DirectX_Shared/Constant_Buffer_Types.h"

namespace Insight {

	class INSIGHT_API APostFx : public AActor
	{
	public:
		APostFx(ActorId id, ActorType type = "Spot Light Actor");
		virtual ~APostFx();

		virtual bool LoadFromJson(const rapidjson::Value& jsonPostFx) override;
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

		inline CB_PS_PostFx GetConstantBuffer() { return m_ShaderCB; }

	private:
		CB_PS_PostFx m_ShaderCB;
		float m_TempInnerRadius = 0.1f;
		float m_TempOuterRadius = 1.0f;
	};

}