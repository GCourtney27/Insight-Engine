#pragma once
#include "Insight/Input/Input.h"
#include "Insight/Input/Keyboard_Buffer.h"
#include "Insight/Input/Mouse_Buffer.h"

namespace Insight {

	class WindowsInput : public Input
	{
	protected:
		virtual bool IsKeyPressedImpl(int keycode) override;

		virtual bool IsMouseButtonPressedImpl(int button) override;
		virtual std::pair<float, float> GetMousePositionImpl() override;
		virtual float GetMouseXImpl() override;
		virtual float GetMouseYImpl() override;
	private:
		KeyboardBuffer m_KeyBoardBuffer;
		MouseBuffer m_MouseBuffer;
	};
}

