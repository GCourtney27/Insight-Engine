#pragma once

#include <Runtime/Core.h>

#include "Runtime/GameFramework/AActor.h"


namespace Insight {

	namespace GameFramework {

		class APlayerCharacter;

		class INSIGHT_API APlayerStart : public AActor
		{
		public:
			APlayerStart(ActorId id, ActorName name = TEXT("Player Start"));
			virtual ~APlayerStart();

			virtual bool OnInit() override;
			virtual bool OnPostInit() override;
			virtual void OnUpdate(const float DeltaMs) override;
			virtual void OnRender() override;

			virtual void BeginPlay() override;
			virtual void Tick(const float DeltaMs) override;

			void RenderSceneHeirarchy() override;
			void OnImGuiRender() override;

			void SpawnPlayer(APlayerCharacter* PlayerCharacter);

		private:
			SceneComponent* m_pSceneComponent = nullptr;
		};

	} // end namespace GameFramework
} // end namespace Insight
