#pragma once

#include <Runtime/Core.h>

#include "Runtime/GameFramework/AActor.h"
#include "Runtime/Graphics/Texture.h"

namespace Insight {

	
	class INSIGHT_API ASkySphere : public GameFramework::AActor
	{
	public:
		ASkySphere(ActorId id, GameFramework::ActorType type = TEXT("Sky Sphere Actor"));
		virtual ~ASkySphere();

		virtual bool LoadFromJson(const rapidjson::Value* jsonSkySphere) override;
		bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>* Writer) override;
		
		virtual bool OnInit();
		virtual bool OnPostInit();
		virtual void OnUpdate(const float DeltaMs);
		virtual void OnPreRender(ieMatrix& parentMat);
		virtual void OnRender();
		virtual void Destroy();

		void RenderSky();


		void OnEvent(Event& e);

		virtual void BeginPlay();
		virtual void Tick(const float DeltaMs);
		virtual void Exit();

		virtual void OnImGuiRender() override;

	private:
		Texture* m_Diffuse;

	};


}
