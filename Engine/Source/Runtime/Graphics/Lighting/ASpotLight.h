#pragma once

#include <Runtime/Core.h>

#include "Runtime/GameFramework/AActor.h"
#include "Platform/DirectXShared/ConstantBufferTypes.h"

namespace Insight {

	class INSIGHT_API ASpotLight : public GameFramework::AActor
	{
	public:
		ASpotLight(ActorId id, GameFramework::ActorType type = TEXT("Spot Light Actor"));
		virtual ~ASpotLight();

		virtual bool LoadFromJson(const rapidjson::Value* jsonSpotLight) override;
		bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>* Writer) override;

		virtual bool OnInit();
		virtual bool OnPostInit();
		virtual void OnUpdate(const float DeltaMs);
		virtual void OnPreRender(ieMatrix& parentMat);
		virtual void OnRender();
		virtual void Destroy();

		void OnEvent(Event& e);

		virtual void BeginPlay();
		virtual void Tick(const float DeltaMs);
		virtual void Exit();

		virtual void OnImGuiRender() override;

		inline CB_PS_SpotLight GetConstantBuffer() { return m_ShaderCB; }
		
	private:
		bool OnEventTranslation(TranslationEvent& e);

	private:
		CB_PS_SpotLight m_ShaderCB;
		float m_TempInnerCutoff = 12.5f;
		float m_TempOuterCutoff = 15.0f;
		GameFramework::SceneComponent* m_pSceneComponent = nullptr;
	};

}