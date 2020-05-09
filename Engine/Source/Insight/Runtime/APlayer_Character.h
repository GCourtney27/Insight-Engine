#pragma once

#include <Insight/Core.h>

#include "Insight/Runtime/APawn.h"
#include "Insight/Runtime/ACamera.h"

namespace Insight {



	class INSIGHT_API APlayerCharacter : public APawn
	{
	public:
		APlayerCharacter();
		~APlayerCharacter();

		inline static APlayerCharacter& Get() { return *s_Instance; }

		virtual void OnInit();
		virtual void OnUpdate();
		virtual void OnRender();

		const Camera& GetCamera() const { return m_Camera; }

	private:
		Camera m_Camera;
		virtual void ProcessInput();
	private:
		static APlayerCharacter* s_Instance;
	};

}
