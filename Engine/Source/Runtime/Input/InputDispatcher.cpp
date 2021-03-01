#include "Engine_pch.h"

#include "InputDispatcher.h"

#include "Runtime/Core/Window.h"

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
			m_AxisMappings.push_back({ "MoveUp", GamepadCode_Trigger_Right, 1.0f });
			m_AxisMappings.push_back({ "MoveUp", GamepadCode_Trigger_Left, -1.0f });

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
			m_ActionMappings.push_back({ "Sprint", GamepadCode_Button_Thumbstick_Left });


			m_GamepadLeftStickSensitivity = 10.0f;
			m_GamepadRightStickSensitivity = 10.0f;
		}

		void InputDispatcher::UpdateInputs(float DeltaMs)
		{
			// Keyboard
			// --------
			// The Window only sends one event telling us the key has been pressed.
			// We need to continuously check it to see if it is being held.
			// We do this below.
			for (uint32_t i = 0; i < m_AxisMappings.size(); i++)
			{
				// If the key in the axis mapping is pressed, dispatch an event.
				if(m_pOwningWindowRef->GetAsyncKeyState(m_AxisMappings[i].MappedKeyCode) == IET_Pressed)
								{
					// Dispatching KeyHolding events will happen in InputDispatcher::DispatchActionEvent
					KeyPressedEvent e(m_AxisMappings[i].MappedKeyCode, 0, 1.0f);
					ProcessInputEvent(e);
				}
			}

			// Gamepad
			// -------
			HandleControllerInput(DeltaMs);

		}

		void InputDispatcher::ProcessInputEvent(Event& e)
		{
			EventDispatcher Dispatcher(e);
			// Mouse Buttons
			Dispatcher.Dispatch<MouseMovedEvent>(IE_BIND_LOCAL_EVENT_FN(InputDispatcher::DispatchMouseMoveEvent));
			Dispatcher.Dispatch<MouseButtonPressedEvent>(IE_BIND_LOCAL_EVENT_FN(InputDispatcher::DispatchActionEvent));
			Dispatcher.Dispatch<MouseButtonReleasedEvent>(IE_BIND_LOCAL_EVENT_FN(InputDispatcher::DispatchActionEvent));
			// Mouse Scroll
			Dispatcher.Dispatch<MouseScrolledEvent>(IE_BIND_LOCAL_EVENT_FN(InputDispatcher::DispatchMouseScrolledEvent));

			// Key Pressed
			Dispatcher.Dispatch<KeyPressedEvent>(IE_BIND_LOCAL_EVENT_FN(InputDispatcher::DispatchAxisEvent));
			Dispatcher.Dispatch<KeyPressedEvent>(IE_BIND_LOCAL_EVENT_FN(InputDispatcher::DispatchActionEvent));
			Dispatcher.Dispatch<KeyReleasedEvent>(IE_BIND_LOCAL_EVENT_FN(InputDispatcher::DispatchActionEvent));

			// Key Typed
			//Dispatcher.Dispatch<KeyTypedEvent>(IE_BIND_EVENT_FN(InputManager::OnKeyTypedEvent));
		}

		void InputDispatcher::RegisterAxisCallback(const char* Name, EventInputAxisFn Callback)
		{
			m_AxisCallbacks[Name].push_back(Callback);
		}

		void InputDispatcher::RegisterActionCallback(const char* Name, EInputEventType EventType, EventInputActionFn Callback)
		{
			m_ActionCallbacks[{Name, EventType}].push_back(Callback);
		}

		void InputDispatcher::AddGamepadVibration(uint32_t PlayerIndex, EGampadRumbleMotor Direction, float Amount)
		{
			IE_ASSERT(PlayerIndex <= XUSER_MAX_COUNT, "Trying to add vibration to an invalid controller index");

			XINPUT_VIBRATION VibrationInfo;
			ZeroMemory(&VibrationInfo, sizeof(XINPUT_VIBRATION));

			if (Direction & GampadRumbleMotor_Left)
				VibrationInfo.wLeftMotorSpeed = static_cast<WORD>(65535 / (65535 * Amount));
			if (Direction & GampadRumbleMotor_Right)
				VibrationInfo.wRightMotorSpeed = static_cast<WORD>(65535 / (65535 * Amount));

			XInputSetState(PlayerIndex, &VibrationInfo);
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
						Callback(Axis.Scale * e.GetMoveDelta());
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
					if (e.GetEventType() == IET_Released)
					{
						Action.CanDispatch = true;
						// The key had been released reset 
						// the holding timer.
						Action.HoldTime = 0.0f;
					}

					// If the key has been pressed once or a key is being held, dispatch the function calls.
					// Note: KeyHeld events are only dispatched from the timer code below.
					if (Action.CanDispatch || (e.GetEventType() == IET_Held))
					{
						auto Callbacks = &m_ActionCallbacks[{Action.Hint, e.GetEventType()}];
						for (EventInputActionFn Callback : *Callbacks)
						{
							// Invoke the callbacks.
							Callback();
						}
					}

					if (e.GetEventType() == IET_Pressed)
					{
						Action.CanDispatch = false;
						// If the the key is pressed start the timer to see if the key is being held.
						Action.HoldTime += 0.16f;
						if (Action.HoldTime >= m_MaxKeyHoldTime)
						{
							// Time is greater than the max hold time 
							// (They have been holding the key for 1/0.16 seconds)
							// meaning they are holding the key. Send out a key hold 
							KeyHeldEvent e(e.GetKeyCode());
							DispatchActionEvent(e);
						}
					}
				}
			}
			return false;
		}

		void InputDispatcher::HandleControllerInput(const float DeltaMs)
		{
			// Constant gamepad polling is poor for performance, so set
			// a poll interval and update the controller every other frame.
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
						if (State.Gamepad.wButtons & XBoxCode_Button_PressMask_A)
						{
							KeyPressedEvent e(GamepadCode_Button_A, 0);
							ProcessInputEvent(e);
						}
						else if (m_XBoxGamepads[i].Gamepad.wButtons & XBoxCode_Button_PressMask_A)
						{
							KeyReleasedEvent e(GamepadCode_Button_A);
							ProcessInputEvent(e);
						}

						if (State.Gamepad.wButtons & XBoxCode_Button_PressMask_B)
						{
							KeyPressedEvent e(GamepadCode_Button_B, 0);
							ProcessInputEvent(e);
						}
						else if (m_XBoxGamepads[i].Gamepad.wButtons & XBoxCode_Button_PressMask_B)
						{
							KeyReleasedEvent e(GamepadCode_Button_B);
							ProcessInputEvent(e);
						}



						if (State.Gamepad.wButtons & XBoxCode_Button_PressMask_X)
						{
							KeyPressedEvent e(GamepadCode_Button_X, 0);
							ProcessInputEvent(e);
						}
						else if (m_XBoxGamepads[i].Gamepad.wButtons & XBoxCode_Button_PressMask_X)
						{
							KeyReleasedEvent e(GamepadCode_Button_X);
							ProcessInputEvent(e);
						}



						if (State.Gamepad.wButtons & XBoxCode_Button_PressMask_Y)
						{
							KeyPressedEvent e(GamepadCode_Button_Y, 0);
							ProcessInputEvent(e);
						}
						else if (m_XBoxGamepads[i].Gamepad.wButtons & XBoxCode_Button_PressMask_Y)
						{
							KeyReleasedEvent e(GamepadCode_Button_Y);
							ProcessInputEvent(e);
						}



						if (State.Gamepad.wButtons & XBoxCode_Button_PressMask_DPad_Up)
						{
							KeyPressedEvent e(GamepadCode_Button_DPad_Up, 0);
							ProcessInputEvent(e);
						}
						else if ((m_XBoxGamepads[i].Gamepad.wButtons & XBoxCode_Button_PressMask_DPad_Up))
						{
							KeyReleasedEvent e(GamepadCode_Button_DPad_Up);
							ProcessInputEvent(e);
						}



						if (State.Gamepad.wButtons & XBoxCode_Button_PressMask_DPad_Down)
						{
							KeyPressedEvent e(GamepadCode_Button_DPad_Down, 0);
							ProcessInputEvent(e);
						}
						else if ((m_XBoxGamepads[i].Gamepad.wButtons & XBoxCode_Button_PressMask_DPad_Down))
						{
							KeyReleasedEvent e(GamepadCode_Button_DPad_Down);
							ProcessInputEvent(e);
						}



						if (State.Gamepad.wButtons & XBoxCode_Button_PressMask_DPad_Left)
						{
							KeyPressedEvent e(GamepadCode_Button_DPad_Left, 0);
							ProcessInputEvent(e);
						}
						else if ((m_XBoxGamepads[i].Gamepad.wButtons & XBoxCode_Button_PressMask_DPad_Left))
						{
							KeyReleasedEvent e(GamepadCode_Button_DPad_Left);
							ProcessInputEvent(e);
						}



						if (State.Gamepad.wButtons & XBoxCode_Button_PressMask_DPad_Right)
						{
							KeyPressedEvent e(GamepadCode_Button_DPad_Right, 0);
							ProcessInputEvent(e);
						}
						else if ((m_XBoxGamepads[i].Gamepad.wButtons & XBoxCode_Button_PressMask_DPad_Right))
						{
							KeyReleasedEvent e(GamepadCode_Button_DPad_Right);
							ProcessInputEvent(e);
						}



						if (State.Gamepad.wButtons & XBoxCode_Button_PressMask_Start)
						{
							KeyPressedEvent e(GamepadCode_Button_Start, 0);
							ProcessInputEvent(e);
						}
						else if ((m_XBoxGamepads[i].Gamepad.wButtons & XBoxCode_Button_PressMask_Start))
						{
							KeyReleasedEvent e(GamepadCode_Button_Start);
							ProcessInputEvent(e);
						}



						if (State.Gamepad.wButtons & XBoxCode_Button_PressMask_Back)
						{
							KeyPressedEvent e(GamepadCode_Button_Back, 0);
							ProcessInputEvent(e);
						}
						else if ((m_XBoxGamepads[i].Gamepad.wButtons & XBoxCode_Button_PressMask_Back))
						{
							KeyReleasedEvent e(GamepadCode_Button_Back);
							ProcessInputEvent(e);
						}



						if (State.Gamepad.wButtons & XBoxCode_Button_PressMask_Thumbstick_Left)
						{
							KeyPressedEvent e(GamepadCode_Button_Thumbstick_Left, 0);
							ProcessInputEvent(e);
						}
						else if ((m_XBoxGamepads[i].Gamepad.wButtons & XBoxCode_Button_PressMask_Thumbstick_Left))
						{
							KeyReleasedEvent e(GamepadCode_Button_Thumbstick_Left);
							ProcessInputEvent(e);
						}



						if (State.Gamepad.wButtons & XBoxCode_Button_PressMask_Thumbstick_Right)
						{
							KeyPressedEvent e(GamepadCode_Button_Thumbstick_Right, 0);
							ProcessInputEvent(e);
						}
						else if ((m_XBoxGamepads[i].Gamepad.wButtons & XBoxCode_Button_PressMask_Thumbstick_Right))
						{
							KeyReleasedEvent e(GamepadCode_Button_Thumbstick_Right);
							ProcessInputEvent(e);
						}



						if (State.Gamepad.wButtons & XBoxCode_Button_PressMask_Shoulder_Left)
						{
							KeyPressedEvent e(GamepadCode_Button_Shoulder_Left, 0);
							ProcessInputEvent(e);
						}
						else if ((m_XBoxGamepads[i].Gamepad.wButtons & XBoxCode_Button_PressMask_Shoulder_Left))
						{
							KeyReleasedEvent e(GamepadCode_Button_Shoulder_Left);
							ProcessInputEvent(e);
						}



						if (State.Gamepad.wButtons & XBoxCode_Button_PressMask_Shoulder_Right)
						{
							KeyPressedEvent e(GamepadCode_Button_Shoulder_Right, 0);
							ProcessInputEvent(e);
						}
						else if ((m_XBoxGamepads[i].Gamepad.wButtons & XBoxCode_Button_PressMask_Shoulder_Right))
						{
							KeyReleasedEvent e(GamepadCode_Button_Shoulder_Right);
							ProcessInputEvent(e);
						}

						// Has the state changed since last poll?
						if (State.dwPacketNumber != m_XBoxGamepads[i].dwPacketNumber)
						{
							m_XBoxGamepads[i] = State;
						}

						// Trigger Left
						float TriggerL = State.Gamepad.bLeftTrigger;
						static bool TriggerLPressed = false;
						if (TriggerL > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
						{
							TriggerLPressed = true;
							float TriggerNormalized = TriggerL / 255;
							//IE_LOG(Log, TEXT("0 - 1: {0}"), TriggerNormalized);
							KeyPressedEvent e(GamepadCode_Trigger_Left, 0, TriggerNormalized);
							ProcessInputEvent(e);
						}
						else
						{
							if (TriggerLPressed)
							{
								TriggerLPressed = false; 
								KeyReleasedEvent e(GamepadCode_Trigger_Left);
								ProcessInputEvent(e);
							}
						}

						// Trigger Right
						float TriggerR = State.Gamepad.bRightTrigger;
						static bool TriggerRPressed = false;
						if (TriggerR > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
						{
							TriggerRPressed = true;
							float TriggerNormalized = TriggerR / 255;
							//IE_LOG(Log, TEXT("0 - 1: {0}"), TriggerNormalized);
							KeyPressedEvent e(GamepadCode_Trigger_Right, 0, TriggerNormalized);
							ProcessInputEvent(e);
						}
						else
						{
							if (TriggerRPressed)
							{
								TriggerRPressed = false;
								KeyReleasedEvent e(GamepadCode_Trigger_Right);
								ProcessInputEvent(e);
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

						float MoveDeltaLY = (normalizedLY * NormalizedDistanceL) * m_GamepadLeftStickSensitivity;
						static bool MoovedLY = false;
						if (MoveDeltaLY != 0.0f)
						{
							MoovedLY = true;
							KeyPressedEvent e(GamepadCode_Thumbstick_Left_Axis_Y, 0, MoveDeltaLY);
							ProcessInputEvent(e);
						}
						else
						{
							if (MoovedLY)
							{
								MoovedLY = false;
								KeyReleasedEvent e(GamepadCode_Thumbstick_Left_Axis_Y);
								ProcessInputEvent(e);
							}
						}

						float MoveDeltaLX = (normalizedLX * NormalizedDistanceL) * m_GamepadLeftStickSensitivity;
						static bool MoovedLX = false;
						if (MoveDeltaLX != 0.0f)
						{
							MoovedLX = true;
							KeyPressedEvent e(GamepadCode_Thumbstick_Left_Axis_X, 0, MoveDeltaLX);
							ProcessInputEvent(e);
						}
						else
						{
							if (MoovedLX)
							{
								MoovedLX = false;
								KeyReleasedEvent e(GamepadCode_Thumbstick_Left_Axis_X);
								ProcessInputEvent(e);
							}
						}

						float MoveDeltaRY = (normalizedRY * NormalizedDistanceR) * m_GamepadRightStickSensitivity;
						static bool MoovedRY = false;
						if (MoveDeltaRY != 0.0f)
						{
							MoovedRY = true;
							KeyPressedEvent e(GamepadCode_Thumbstick_Right_Axis_Y, 0, MoveDeltaRY);
							ProcessInputEvent(e);
						}
						else
						{
							if (MoovedRY)
							{
								MoovedRY = false;
								KeyReleasedEvent e(GamepadCode_Thumbstick_Right_Axis_Y);
								ProcessInputEvent(e);
							}
						}

						float MoveDeltaRX = (normalizedRX * NormalizedDistanceR) * m_GamepadRightStickSensitivity;
						static bool MoovedRX = false;
						if (MoveDeltaRX != 0.0f)
						{
							MoovedRX = true;
							KeyPressedEvent e(GamepadCode_Thumbstick_Right_Axis_X, 0, MoveDeltaRX);
							ProcessInputEvent(e);
						}
						else
						{
							if (MoovedRX)
							{
								MoovedRX = false;
								KeyReleasedEvent e(GamepadCode_Thumbstick_Right_Axis_X);
								ProcessInputEvent(e);
							}
						}
					}
					else
					{
						// Controller is not connected. Dont do anything.
					}
				}
			}
		}
		
		
	}

}
