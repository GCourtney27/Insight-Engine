#pragma once

#include <Insight/Core.h>

#include "Insight/Runtime/APawn.h"
#include "Insight/Runtime/ACamera.h"

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
				ViewTarget.Position += GetTransform().GetPosition();
				return ViewTarget;
			}

			virtual bool OnInit() override;
			virtual void OnUpdate(const float DeltaMs) override;
			virtual void CalculateParent(XMMATRIX parentMat) override;
			virtual void OnRender() override;
			virtual void Tick(const float DeltaMs) override;
			void RenderSceneHeirarchy() override;
			void OnImGuiRender() override;

		private:
			ViewTarget m_ViewTarget;
			ACamera* m_pCamera;
			virtual void ProcessInput(const float DeltaMs);
		private:
			static APlayerCharacter* s_Instance;
		};

	} // end namespace Runtime
} // end namespace Insight
