#include "Engine_pch.h"

#include "Input_Dispatcher.h"

namespace Insight {

	namespace Input {

		InputDispatcher* InputDispatcher::s_Instance = nullptr;

		InputDispatcher::InputDispatcher()
			: m_GamepadPollInterval(0.01f)
		{
			s_Instance = this;

			// TODO load these from a settings file
			// TODO Change axis mappings depending on play mode (Editor/InGame)
			m_AxisMappings.push_back({ "MoveForward", KeyMapCode_Keyboard_W, 1.0f });
			m_AxisMappings.push_back({ "MoveForward", KeyMapCode_Keyboard_S, -1.0f });
			m_AxisMappings.push_back({ "MoveForward", GamepadCode_Thumbstick_Left_Axis_Y, 1.0f });
			m_AxisMappings.push_back({ "MoveRight", KeyMapCode_Keyboard_D, 1.0f });
			m_AxisMappings.push_back({ "MoveRight", KeyMapCode_Keyboard_A, -1.0f });
			m_AxisMappings.push_back({ "MoveRight", GamepadCode_Thumbstick_Left_Axis_X, 1.0f });
			m_AxisMappings.push_back({ "MoveUp", KeyMapCode_Keyboard_E, 1.0f });
			m_AxisMappings.push_back({ "MoveUp", KeyMapCode_Keyboard_Q, -1.0f });

			m_AxisMappings.push_back({ "LookUp", KeyMapCode_Mouse_MoveY, 1.0f });
			m_AxisMappings.push_back({ "LookUp", KeyMapCode_Mouse_MoveY, -1.0f });
			m_AxisMappings.push_back({ "LookUp", GamepadCode_Thumbstick_Right_Axis_Y, -1.0f });
			m_AxisMappings.push_back({ "LookRight", KeyMapCode_Mouse_MoveX, 1.0f });
			m_AxisMappings.push_back({ "LookRight", KeyMapCode_Mouse_MoveX, -1.0f });
			m_AxisMappings.push_back({ "LookRight", GamepadCode_Thumbstick_Right_Axis_X, 1.0f });
			m_AxisMappings.push_back({ "MouseWheelUp", KeyMapCode_Mouse_Wheel_Up, 1.0f });
			m_AxisMappings.push_back({ "MouseWheelUp", KeyMapCode_Mouse_Wheel_Up, -1.0f });

			m_ActionMappings.push_back({ "CameraPitchYawLock", KeyMapCode_Mouse_Button_Right });
			m_ActionMappings.push_back({ "CameraPitchYawLock", GamepadCode_Thumbstick_Right_Axis_X });
			m_ActionMappings.push_back({ "Sprint", KeyMapCode_Keyboard_Shift });
			m_ActionMappings.push_back({ "Sprint", GamepadCode_Button_A });


			m_GamepadLeftStickSensitivity = 20.0f;
			m_GamepadRightStickSensitivity = 20.0f;
		}

		void InputDispatcher::UpdateInputs(float DeltaMs)
		{
			for (uint32_t i = 0; i < m_AxisMappings.size(); i++)
			{
				SHORT KeyState = ::GetAsyncKeyState(m_AxisMappings[i].MappedKeyCode);
				bool Pressed = (BIT_SHIFT(15)) & KeyState;
				if (Pressed)
				{
					KeyPressedEvent e(m_AxisMappings[i].MappedKeyCode, 0, 1.0f);
					ProcessInputEvent(e);
				}
			}


			static float GamepadPollRate = 0.0f;
			GamepadPollRate += DeltaMs;
			if (GamepadPollRate >= m_GamepadPollInterval)
			{
				GamepadPollRate = 0.0f;

				DWORD dwResult;
				for (UINT i = 0; i < XUSER_MAX_COUNT; ++i)
				{
					XINPUT_STATE State;
					ZeroMemory(&State, sizeof(XINPUT_STATE));

					dwResult = XInputGetState(i, &State);

					if (dwResult == ERROR_SUCCESS)
					{
						// Controller is connected

						// Has the state changed since last poll?
						if (State.dwPacketNumber != m_XBoxGamepads[i].dwPacketNumber)
						{
							m_XBoxGamepads[i] = State;
							if (State.Gamepad.wButtons & XBoxCode_Button_A)
							{
								for (uint32_t j = 0; j < m_ActionMappings.size(); j++)
								{
									if (m_ActionMappings[j].MappedKeyCode == GamepadCode_Button_A)
									{
										KeyPressedEvent e(m_ActionMappings[j].MappedKeyCode, 0);
										ProcessInputEvent(e);
									}
								}
							}

						}

						// Thumbstick Left
						float LeftX = State.Gamepad.sThumbLX;
						float LeftY = State.Gamepad.sThumbLY;

						float MoveDistanceL = sqrt(LeftX * LeftX + LeftY * LeftY);

						float normalizedLX = LeftX / MoveDistanceL;
						float normalizedLY = LeftY / MoveDistanceL;

						float NormalizedDistanceL = 0.0f;

						if (MoveDistanceL > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
						{
							if (MoveDistanceL > 32767) MoveDistanceL = 32767;

							MoveDistanceL -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
							NormalizedDistanceL = MoveDistanceL / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
						}
						else
						{
							MoveDistanceL = 0.0f;
							NormalizedDistanceL = 0.0f;
							normalizedLY = 0.0f;
							normalizedLX = 0.0f;
						}
						// Thumbstick Right
						float RightX = State.Gamepad.sThumbRX;
						float RightY = State.Gamepad.sThumbRY;

						float MoveDistanceR = sqrt(RightX * RightX + RightY * RightY);

						float normalizedRX = RightX / MoveDistanceR;
						float normalizedRY = RightY / MoveDistanceR;

						float NormalizedDistanceR = 0.0f;

						if (MoveDistanceR > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
						{
							if (MoveDistanceR > 32767) MoveDistanceR = 32767;

							MoveDistanceR -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
							NormalizedDistanceR = MoveDistanceR / (32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
						}
						else
						{
							MoveDistanceR = 0.0f;
							NormalizedDistanceR = 0.0f;
							normalizedRY = 0.0f;
							normalizedRX = 0.0f;
						}

						for (uint32_t j = 0; j < m_AxisMappings.size(); j++)
						{
							if (m_AxisMappings[j].MappedKeyCode == GamepadCode_Thumbstick_Left_Axis_Y)
							{
								float MoveDelta = (normalizedLY * NormalizedDistanceL) * m_GamepadLeftStickSensitivity;
								KeyPressedEvent e(m_AxisMappings[j].MappedKeyCode, 0, MoveDelta);
								ProcessInputEvent(e);
							}
							else if (m_AxisMappings[j].MappedKeyCode == GamepadCode_Thumbstick_Left_Axis_X)
							{
								float MoveDelta = (normalizedLX * NormalizedDistanceL) * m_GamepadLeftStickSensitivity;
								KeyPressedEvent e(m_AxisMappings[j].MappedKeyCode, 0, MoveDelta);
								ProcessInputEvent(e);
							}
							else if(m_AxisMappings[j].MappedKeyCode == GamepadCode_Thumbstick_Right_Axis_Y)
							{
								float MoveDelta = (normalizedRY * NormalizedDistanceR) * m_GamepadRightStickSensitivity;
								KeyPressedEvent e(m_AxisMappings[j].MappedKeyCode, 0, MoveDelta);
								ProcessInputEvent(e);
							}
							else if (m_AxisMappings[j].MappedKeyCode == GamepadCode_Thumbstick_Right_Axis_X)
							{
								float MoveDelta = (normalizedRX * NormalizedDistanceR) * m_GamepadRightStickSensitivity;
								KeyPressedEvent e(m_AxisMappings[j].MappedKeyCode, 0, MoveDelta);
								ProcessInputEvent(e);
							}
						}

						
					}
					else
					{
						// Controller is not connected
					}
				}
			}

		}

		void InputDispatcher::ProcessInputEvent(Event& e)
		{
			EventDispatcher Dispatcher(e);
			// Mouse Buttons
			Dispatcher.Dispatch<MouseMovedEvent>(IE_BIND_EVENT_FN(InputDispatcher::DispatchMouseMoveEvent));
			Dispatcher.Dispatch<MouseButtonPressedEvent>(IE_BIND_EVENT_FN(InputDispatcher::DispatchActionEvent));
			Dispatcher.Dispatch<MouseButtonReleasedEvent>(IE_BIND_EVENT_FN(InputDispatcher::DispatchActionEvent));
			// Mouse Scroll
			Dispatcher.Dispatch<MouseScrolledEvent>(IE_BIND_EVENT_FN(InputDispatcher::DispatchMouseScrolledEvent));

			// Key Pressed
			Dispatcher.Dispatch<KeyPressedEvent>(IE_BIND_EVENT_FN(InputDispatcher::DispatchAxisEvent));
			Dispatcher.Dispatch<KeyPressedEvent>(IE_BIND_EVENT_FN(InputDispatcher::DispatchActionEvent));
			Dispatcher.Dispatch<KeyReleasedEvent>(IE_BIND_EVENT_FN(InputDispatcher::DispatchActionEvent));

			// Key Typed
			//Dispatcher.Dispatch<KeyTypedEvent>(IE_BIND_EVENT_FN(InputManager::OnKeyTypedEvent));
		}

		void InputDispatcher::RegisterAxisCallback(const char* Name, EventInputAxisFn Callback)
		{
			m_AxisCallbacks[Name].push_back(Callback);
		}

		void InputDispatcher::RegisterActionCallback(const char* Name, InputEventType EventType, EventInputActionFn Callback)
		{
			m_ActionCallbacks[{Name, EventType}].push_back(Callback);
		}

		bool InputDispatcher::DispatchAxisEvent(KeyPressedEvent& e)
		{
			for (AxisMapping& Axis : m_AxisMappings)
			{
				// Find the key in the axis map.
				if (Axis.MappedKeyCode == e.GetKeyCode())
				{
					// Use the keycode as the key into the axis map to find 
					// the callbacks associated with it.
					auto Callbacks = &m_AxisCallbacks[Axis.Hint];
					for (EventInputAxisFn Callback : *Callbacks)
					{
						// Call the callbacks.
						float TotalDelta = Axis.Scale * e.GetMoveDelta();
						Callback(TotalDelta);
					}
				}
			}
			return false;
		}

		bool InputDispatcher::DispatchMouseScrolledEvent(MouseScrolledEvent& e)
		{
			for (AxisMapping& Axis : m_AxisMappings)
			{
				// Find the key in the axis map.
				if (Axis.MappedKeyCode == e.GetKeyCode())
				{
					float MoveFactor = e.GetYOffset();

					// Use the keycode as the key into the axis map to find 
					// the callbacks associated with it.
					auto Callbacks = &m_AxisCallbacks[Axis.Hint];
					for (EventInputAxisFn Callback : *Callbacks)
					{
						// Invoke the callbacks.
						Callback(MoveFactor);
					}
				}
			}
			return false;
		}

		bool InputDispatcher::DispatchMouseMoveEvent(MouseMovedEvent& e)
		{
			for (AxisMapping& Axis : m_AxisMappings)
			{
				// Find the key in the axis map.
				if (Axis.MappedKeyCode == e.GetKeyCode())
				{
					float MoveFactor = 0.0f;
					if (e.GetKeyCode() == KeyMapCode_Mouse_MoveX) MoveFactor = e.GetX();
					if (e.GetKeyCode() == KeyMapCode_Mouse_MoveY) MoveFactor = e.GetY();

					// Use the keycode as the key into the axis map to find 
					// the callbacks associated with it.
					auto Callbacks = &m_AxisCallbacks[Axis.Hint];
					for (EventInputAxisFn Callback : *Callbacks)
					{
						// Invoke the callbacks.
						Callback(MoveFactor);
					}
				}
			}
			return false;
		}

		bool InputDispatcher::DispatchActionEvent(InputEvent& e)
		{
			for (ActionMapping& Action : m_ActionMappings)
			{
				if (Action.MappedKeyCode == e.GetKeyCode())
				{
					if (e.GetEventType() == InputEventType_Released)
					{
						Action.CanDispatch = true;
					}

					if (Action.CanDispatch)
					{
						auto Callbacks = &m_ActionCallbacks[{Action.Hint, e.GetEventType()}];
						for (EventInputActionFn Callback : *Callbacks)
						{
							// Invoke the callbacks.
							Callback();
						}
					}

					if (e.GetEventType() == InputEventType_Pressed)
					{
						Action.CanDispatch = false;
					}
				}
			}
			return false;
		}
		
		float InputDispatcher::GetGamepadThumbstickMoveDelta(uint8_t Axis, bool GetLeftValue)
		{
			float MoveDelta = 0.0f;


			return MoveDelta;
		}
	}

}
