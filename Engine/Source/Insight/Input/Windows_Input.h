#pragma once
#include "Insight/Input/Input.h"
#include "Insight/Input/Keyboard_Buffer.h"
#include "Insight/Input/Mouse_Buffer.h"

namespace Insight {

	class WindowsInput : public Input
	{
	public:
		WindowsInput() {}
		~WindowsInput() {}

	protected:
		virtual bool IsKeyPressedImpl(int keycode) override;

		virtual bool IsMouseButtonPressedImpl(int button) override;
		virtual std::pair<float, float> GetMousePositionImpl() override;
		virtual std::pair<int, int> GetRawMousePositionImpl() override;
		virtual float GetMouseXImpl() override;
		virtual float GetMouseYImpl() override;
		virtual bool IsMouseScrolledImpl() override;
		virtual std::pair<float, float> GetMouseScrollOffsetImpl() override;
	};
}

