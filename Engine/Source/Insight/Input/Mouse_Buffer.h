#pragma once

#include <Insight/Core.h>

#include "Insight/Input/Input_Buffer.h"

namespace Insight {

	class INSIGHT_API MouseBuffer : public InputBuffer
	{
	public:
		MouseBuffer();
		~MouseBuffer();

		inline void GetMousePosition(double& xPos, double& yPos) { xPos = m_xPos; yPos = m_yPos; }
		inline void GetRawMousePosition(int& xPos, int& yPos) { xPos = m_xPosRaw; yPos = m_yPosRaw; }
		KeyPressState GetButtonState(const BYTE button);
		void OnButtonPressed(const BYTE button);
		void OnButtonReleased(const BYTE button);
		void OnMouseScroll(float xOffset, float yOffset);
		MouseScrollState GetScrollWheelState();
		inline void GetMouseScrollOffset(float& xOffset, float& yOffset) { xOffset = m_MouseScrollXOffset; yOffset = m_MouseScrollYOffset; }
		inline void OnMousePositionMoved(double newXPos, double newYPos) { m_xPos = newXPos; m_yPos = newYPos;	}
		inline void OnRawMousePositionMoved(int newXPos, int newYPos) { m_xPosRaw = newXPos; m_yPosRaw = newYPos;	}
	private:
		int m_xPosRaw = 0.0;
		int m_yPosRaw = 0.0;
		double m_xPos = 0.0;
		double m_yPos = 0.0;
		bool m_Buttons[3] = { false };
		bool m_ScrollState[4] = { false };
		float m_MouseScrollXOffset = 0.0f;
		float m_MouseScrollYOffset = 0.0f;
	};

}
