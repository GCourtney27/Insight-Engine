// Copyright Insight Interactive. All Rights Reserved.
/*
	File - Input_Dispatcher.h
	Source - Input_Dispatcher.cpp

	Author - Garrett Courtney

	Purpose:
	Core input system for the engine giving functionality to Actors and their place in the world.

	Description:
	Mapping profiles are created and components request member functions to be called when input
	events are fulfill the profiles needs. Actor InputComponents (Input_Component.h) bind axis
	and actions for this to request this functionality.

	Example Usage:
	ActionMapping::Hint = "Jump"
	ActionMapping::MappedKeycode = KeyMapCode_Keyboard_Space
	ActionMapping::CanDispatch = true;
	-	Any components that binds a member function to this ActionMapping will be called.
		Ex. pMyInputComponent->BindAction("Jump", InputEventType_Pressed, IE_BIND_VOID_FN(MyClass::DoJump));
			- Method "DoJump" will be called in class named "MyClass" when the space bar is pressed.

*/
#pragma once

#include <Runtime/Core.h>

#include "Runtime/Core/Public/Events/Event.h"
#include "Runtime/Core/Public/Events/KeyEvent.h"
#include "Runtime/Core/Public/Events/MouseEvent.h"

// For XBox Controllers
#include <Xinput.h>

namespace Insight {

	namespace Input {



		constexpr uint8_t MaxHintStringLength = 32u;

		// The required function signature an input component's axis binding should use.
		using EventInputAxisFn = std::function<void(float)>;
		// The required function signature an input component's action binding should use.
		using EventInputActionFn = std::function<void(void)>;

		/*
			Describes an axis mapping that input components can use to recieve events on.
			@param Hint: The name to associate an event with.
			@param MappedKeyCode: The keycode that will trigger the event callbacks attached to this axis mapping
			@param Scale: The value to pass to callback functions.
		*/
		struct AxisMapping
		{
			char		Hint[MaxHintStringLength];
			KeyMapCode	MappedKeyCode;
			float		Scale;
		};

		/*
			Describes an action mapping that input components can use to recieve events on.
			@param Hint: The name to associate an event with.
			@param MappedKeyCode: The keycode that will trigger the event callbacks attached to this axis mapping
			@param CanDispatch: Internal variable used to determine wether to send a "released" event once a
								"pressed" event has been sent.
			@parame HoldTime: Internal counter variable that keeps track of how much time the mapped key has been 
							  held down. If greater than InputDispatcher::m_MaxKeyHoldTime, InputEventType::InputEventType_Held 
							  events will be dispatched for this action.
		*/
		struct ActionMapping
		{
			char		Hint[MaxHintStringLength];
			KeyMapCode	MappedKeyCode;
			bool		CanDispatch = true;
			float		HoldTime;
		};

		class INSIGHT_API InputDispatcher
		{
		public:
			InputDispatcher();
			~InputDispatcher() = default;

			/*
				Get this instance of the Input Dispatcher.
			*/
			static InputDispatcher& Get() { return *s_Instance; }

			/*
				Sets a referece to the owning window this application uses and dispaches events from.
			*/
			void SetWindowRef(std::shared_ptr<Window> pWindow) { m_pOwningWindowRef = pWindow; }

			/*
				Updates the keyboard axis mappings with the OS.
			*/
			void UpdateInputs(float DeltaMs);
			/*
				Decodes and sends events to approbriate Dispatch* functions.
			*/
			void ProcessInputEvent(Event& e);

			/*
				Register an Axis callback function. Used by Actor input components.
			*/
			void RegisterAxisCallback(const char* Name, EventInputAxisFn Callback);

			/*
				Register an Action callback function. Used by Actor input components.
			*/
			void RegisterActionCallback(const char* Name, EInputEventType EventType, EventInputActionFn Callback);

			/*
				Adds vibration to a controller at a player index.
				@param PlayerIndex: The index of the player to add vibration to.
				@param Motor: The motor to add vibration to. This can be either the left or right gamepad motor.
				@param Amount: A normalized value (0 - 1) that specifies the amount of vibration to add to the controller. 0 being no vibration and 1 being full vibration.
			*/
			void AddGamepadVibration(uint32_t PlayerIndex, EGampadRumbleMotor Direction, float Amount);

		private:
			/*
				Dispatch an event to all callbacks that request a key press event.
			*/
			bool DispatchAxisEvent(KeyPressedEvent& e);

			/*
				Dispatch an event to all callbacks that request a mouse scroll event.
			*/
			bool DispatchMouseScrolledEvent(MouseScrolledEvent& e);
			/*
				Dispatch an event to all callbacks that request a mouse move event.
			*/
			bool DispatchMouseMoveEvent(MouseMovedEvent& e);
			/*
				Dispatch an event to all callbacks that request an action event.
			*/
			bool DispatchActionEvent(InputEvent& e);
			/*
				Handles all controller input and dispatches events to the appropriate 
				functions when necessary.
				@param DeltaMs: World delta time.
			*/
			void HandleControllerInput(const float DeltaMs);


		private:
			std::shared_ptr<Window> m_pOwningWindowRef;
			// Holds all axis mapping profiles.
			std::vector<AxisMapping> m_AxisMappings;
			// Holds all callback funcitons and their corisponding hints found the Axis mappings stored in InputDispatcher::m_AxisMappings.
			std::map<std::string, std::vector<EventInputAxisFn>> m_AxisCallbacks;
			// Holds all action mapping profiles.
			std::vector<ActionMapping> m_ActionMappings;
			// Holds all callback funcitons and their corisponding hints found the actoin mappings stored in InputDispatcher::m_ActionMappings.
			std::map<std::pair<std::string, EInputEventType>, std::vector<EventInputActionFn>> m_ActionCallbacks;

			// Max amount of time the user pressed a key before it is recognized as being held.
			float m_MaxKeyHoldTime = 1.0f;
			// Input states for all XBox controllers.
			XINPUT_STATE m_XBoxGamepads[XUSER_MAX_COUNT];
			// The interval in which to poll controllers and update their input state. This should be kep to a small value and never be 0.
			float m_GamepadPollInterval;

			// TODO Expose these to the user to modify
			float m_GamepadLeftStickSensitivity;
			float m_GamepadRightStickSensitivity;

		private:
			// Static instance of the Input Dispatcher.
			static InputDispatcher* s_Instance;
		};
	}
}