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
		inline void GetRawMousePosition(double& xPos, double& yPos) { xPos = this->xPosRaw; yPos = this->yPosRaw; }
		KeyPressState GetButtonState(const BYTE button);
		void OnButtonPressed(const BYTE button);
		void OnButtonReleased(const BYTE button);
		inline void OnMousePositionMoved(double newXPos, double newYPos) {	xPos = newXPos; yPos = newYPos;	}
		inline void OnRawMousePositionMoved(double newXPos, double newYPos) {	xPosRaw = newXPos; yPosRaw = newYPos;	}
	
		void FlushInputValues() { xPosRaw = 0.0; yPosRaw = 0; }
	private:
		double xPosRaw = 0.0;
		double yPosRaw = 0.0;
		double xPos = 0.0;
		double yPos = 0.0;
		bool m_Buttons[3] = { false };
	};

}
