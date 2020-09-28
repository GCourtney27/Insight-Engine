#pragma once

#include <Retina/Core.h>

#include "Retina/Runtime/AActor.h"


namespace Retina {

	namespace Runtime {

		class APlayerCharacter;

		class RETINA_API APlayerStart : public AActor
		{
		public:
			APlayerStart(ActorId id, ActorName name = "Player Start");
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

	} // end namespace Runtime
} // end namespace Retina
