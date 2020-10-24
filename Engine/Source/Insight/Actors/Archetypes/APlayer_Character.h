#pragma once

#include <Insight/Core.h>

#include "Insight/Actors/Archetypes/APawn.h"
#include "Insight/Actors/Archetypes/ACamera.h"

namespace Insight {

	namespace Runtime {


		class INSIGHT_API APlayerCharacter : public APawn
		{
		public:
			APlayerCharacter(ActorId id, ActorName name = "Player Character");
			virtual ~APlayerCharacter();

			inline static APlayerCharacter& Get() { return *s_Instance; }
			inline ViewTarget GetViewTarget()
			{
				ViewTarget ViewTarget = m_ViewTarget;
				ViewTarget.Position += m_pCharacterRoot->GetPosition();
				return ViewTarget;
			}

			ieVector3 GetPosition() const { return m_pCharacterRoot->GetPosition(); }
			ieVector3 GetRotation() const { return m_pCharacterRoot->GetRotation(); }
			void SetPosition(float X, float Y, float Z) { m_pCharacterRoot->SetPosition(X, Y, Z); }
			void SetRotation(float X, float Y, float Z) { m_pCharacterRoot->SetRotation(X, Y, Z); }

			virtual bool OnInit() override;
			virtual void OnUpdate(const float DeltaMs) override;
			virtual void OnRender() override;
			virtual void Tick(const float DeltaMs) override;
			void RenderSceneHeirarchy() override;
			void OnImGuiRender() override;

		private:
			virtual void ProcessInput(const float DeltaMs);
		private:
			ViewTarget m_ViewTarget;
			ACamera* m_pCamera;
			SceneComponent* m_pCharacterRoot = nullptr;
		private:
			static APlayerCharacter* s_Instance;
		};

	} // end namespace Runtime
} // end namespace Insight
