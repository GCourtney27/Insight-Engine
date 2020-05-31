#pragma once

#include <Insight/Core.h>

#include "Insight/Runtime/APawn.h"
#include "Insight/Runtime/ACamera.h"

namespace Insight {



	class INSIGHT_API APlayerCharacter : public APawn
	{
	public:
		APlayerCharacter(ActorId id, ActorName name = "Player Character");
		virtual ~APlayerCharacter();

		inline static APlayerCharacter& Get() { return *s_Instance; }

		virtual bool OnInit() override;
		virtual void OnUpdate(const float& deltaMs) override;
		virtual void OnPreRender(XMMATRIX parentMat) override;
		virtual void OnRender() override;
		void RenderSceneHeirarchy() override;

		ACamera& GetCameraRef() { return *m_Camera; }

	private:
		ACamera* m_Camera;
		virtual void ProcessInput(const float& deltaMs);
	private:
		static APlayerCharacter* s_Instance;
	};

}
