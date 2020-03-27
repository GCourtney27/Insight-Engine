#pragma once

#include "Insight/Core.h"

#include "InputBuffer.h"

namespace Insight {

	class INSIGHT_API MouseBuffer : public InputBuffer
	{
	public:
		MouseBuffer();
		~MouseBuffer();

		inline void GetMousePosition(double& xPos, double& yPos) { xPos = this->xPos; yPos = this->yPos; }
		KeyPressState GetButtonState(const BYTE button);
		void OnButtonPressed(const BYTE button);
		void OnButtonReleased(const BYTE button);
		inline void OnMousePositionMoved(double newXPos, double newYPos) { xPos = newXPos; yPos = newYPos; }

		inline static MouseBuffer& Get() { return *s_Instance; }
	private:
		double xPos = 0.0;
		double yPos = 0.0;
		bool m_Buttons[3] = { false };
	private:
		static MouseBuffer* s_Instance;
	};

}
