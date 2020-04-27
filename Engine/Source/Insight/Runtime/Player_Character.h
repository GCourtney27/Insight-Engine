#pragma once

#include "Insight/Core.h"

#include "Insight/Runtime/Pawn.h"
#include "Insight/Runtime/Camera.h"

namespace Insight {



	class INSIGHT_API PlayerCharacter : public Pawn
	{
	public:
		PlayerCharacter();
		~PlayerCharacter();

		inline static PlayerCharacter& Get() { return *s_Instance; }

		virtual void OnInit();
		virtual void OnUpdate();
		virtual void OnRender();

	private:
		Camera m_Camera;
		virtual void ProcessInput();
	private:
		static PlayerCharacter* s_Instance;
	};

}
