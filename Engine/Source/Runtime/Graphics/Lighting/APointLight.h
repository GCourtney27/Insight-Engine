#pragma once

#include "EngineDefines.h"

#include "GameFramework/AActor.h"
#include "Platform/DirectXShared/ConstantBufferTypes.h"

namespace Insight {

	namespace GameFramework {
		class SceneComponent;
	}

	class INSIGHT_API APointLight : public GameFramework::AActor
	{
	public:
		APointLight(ActorId id, GameFramework::ActorType type = TEXT("Point Light Actor"));
		virtual ~APointLight();

		virtual bool LoadFromJson(const rapidjson::Value* jsonPointLight) override;
		bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>* Writer) override;

		virtual bool OnInit();
		virtual bool OnPostInit();
		virtual void OnUpdate(const float DeltaMs);
		virtual void OnRender();
		virtual void Destroy();

		void OnEvent(Event& e);

		virtual void BeginPlay();
		virtual void Tick(const float DeltaMs);
		virtual void Exit();

		virtual void OnImGuiRender() override;

		inline CB_PS_PointLight GetConstantBuffer() { return m_ShaderCB; }

	private:
		bool OnEventTranslation(TranslationEvent& e);
	private:
		CB_PS_PointLight m_ShaderCB;
		GameFramework::SceneComponent* m_pSceneComponent = nullptr;
	};

}