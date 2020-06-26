#pragma once

#include <Insight/Core.h>

#include "Insight/Runtime/AActor.h"


namespace Insight {


	class APlayerCharacter;

	class INSIGHT_API APlayerStart : public AActor
	{
	public:
		APlayerStart(ActorId id, ActorName name = "Player Start");
		virtual ~APlayerStart();

		virtual bool OnInit() override;
		virtual bool OnPostInit() override;
		virtual void OnUpdate(const float& deltaMs) override;
		virtual void OnPreRender(XMMATRIX parentMat) override;
		virtual void OnRender() override;

		virtual void BeginPlay() override;
		virtual void Tick(const float& DeltaMs) override;

		void RenderSceneHeirarchy() override;
		void OnImGuiRender() override;

		void SpawnPlayer(APlayerCharacter* PlayerCharacter);

	private:

	};

}
