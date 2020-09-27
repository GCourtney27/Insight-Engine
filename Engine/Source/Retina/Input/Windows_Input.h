#pragma once
#include "Retina/Input/Input.h"
#include "Retina/Input/Keyboard_Buffer.h"
#include "Retina/Input/Mouse_Buffer.h"

namespace Retina {

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

