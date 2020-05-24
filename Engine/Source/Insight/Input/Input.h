#pragma once

#include <Insight/Core.h>

#include "Insight/Input/Input_Manager.h"
#include "Insight/Input/Key_Codes.h"

namespace Insight {

	class INSIGHT_API Input
	{
	public:
		Input() {}

		inline static InputManager& GetInputManager() { return s_Instance->m_InputManager; }

		inline static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }

		inline static bool IsMouseButtonPressed(int button) { return s_Instance->IsMouseButtonPressedImpl(button); }
		inline static std::pair<float, float> GetMousePosition() { return s_Instance->GetMousePositionImpl(); }
		inline static std::pair<int, int> GetRawMousePosition() { return s_Instance->GetRawMousePositionImpl(); }
		inline static float GetMouseX() { return s_Instance->GetMouseXImpl(); }
		inline static float GetMouseY() { return s_Instance->GetMouseYImpl(); }
		inline static bool IsMouseScolled() { return s_Instance->IsMouseScrolledImpl(); }
		inline static std::pair<float, float> GetMouseScrollOffset() { return s_Instance->GetMouseScrollOffsetImpl(); }
	protected:
		virtual bool IsKeyPressedImpl(int keycode) = 0;

		virtual bool IsMouseButtonPressedImpl(int keycode) = 0;
		virtual std::pair<float, float> GetMousePositionImpl() = 0;
		virtual std::pair<int, int> GetRawMousePositionImpl() = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;
		virtual bool IsMouseScrolledImpl() = 0;
		virtual std::pair<float, float> GetMouseScrollOffsetImpl() = 0;
	protected:
		InputManager m_InputManager;
	private:
		static Input* s_Instance;
	};

}