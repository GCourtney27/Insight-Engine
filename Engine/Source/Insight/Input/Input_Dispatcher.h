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

#include <Insight/Core.h>

#include "Insight/Events/Event.h"
#include "Insight/Events/Key_Event.h"
#include "Insight/Events/Mouse_Event.h"

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
			const char	Hint[MaxHintStringLength];
			KeyMapCode	MappedKeyCode;
			float		Scale;
		};

		/*
			Describes an action mapping that input components can use to recieve events on.
			@param Hint: The name to associate an event with.
			@param MappedKeyCode: The keycode that will trigger the event callbacks attached to this axis mapping
			@param CanDispatch: Internal variable used to determine wether to send a "released" event once a
						   "pressed" event has been sent.
		*/
		struct ActionMapping
		{
			const char	Hint[MaxHintStringLength];
			KeyMapCode	MappedKeycode;
			bool		CanDispatch = true;
		};

		class INSIGHT_API InputDispatcher
		{
		public:
			InputDispatcher()
			{
				s_Instance = this;

				// TODO load these from a settings file
				// TODO Change axis mappings depending on play mode (Editor/InGame)
				m_AxisMappings.reserve(12);
				m_AxisMappings.push_back({ "MoveForward", KeyMapCode_Keyboard_W, 1.0f });
				m_AxisMappings.push_back({ "MoveForward", KeyMapCode_Keyboard_S, -1.0f });
				m_AxisMappings.push_back({ "MoveRight", KeyMapCode_Keyboard_D, 1.0f });
				m_AxisMappings.push_back({ "MoveRight", KeyMapCode_Keyboard_A, -1.0f });
				m_AxisMappings.push_back({ "MoveUp", KeyMapCode_Keyboard_E, 1.0f });
				m_AxisMappings.push_back({ "MoveUp", KeyMapCode_Keyboard_Q, -1.0f });

				m_AxisMappings.push_back({ "LookUp", KeyMapCode_Mouse_MoveY, 1.0f });
				m_AxisMappings.push_back({ "LookUp", KeyMapCode_Mouse_MoveY, -1.0f });
				m_AxisMappings.push_back({ "LookRight", KeyMapCode_Mouse_MoveX, 1.0f });
				m_AxisMappings.push_back({ "LookRight", KeyMapCode_Mouse_MoveX, -1.0f });
				m_AxisMappings.push_back({ "MouseWheelUp", KeyMapCode_Mouse_Wheel_Up, 1.0f });
				m_AxisMappings.push_back({ "MouseWheelUp", KeyMapCode_Mouse_Wheel_Up, -1.0f });

				m_ActionMappings.reserve(2);
				m_ActionMappings.push_back({ "CameraPitchYawLock", KeyMapCode_Mouse_Button_Right });
				m_ActionMappings.push_back({ "Sprint", KeyMapCode_Keyboard_Shift });

			}
			~InputDispatcher() = default;

			/*
				Get this instance of the Input Dispatcher.
			*/
			static InputDispatcher& Get() { return *s_Instance; }

			/*
				Updates the keyboard axis mappings with the OS.
			*/
			void UpdateInputs();
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
			void RegisterActionCallback(const char* Name, InputEventType EventType, EventInputActionFn Callback);


		private:
			/*
				Dispatch an event to all callbacks that request a key press event.
			*/
			bool DispatchKeyPressEvent(KeyPressedEvent& e);

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

		private:
			// Holds all axis mapping profiles.
			std::vector<AxisMapping> m_AxisMappings;
			// Holds all callback funcitons and their corisponding hints found the Axis mappings stored in InputDispatcher::m_AxisMappings.
			std::map<std::string, std::vector<EventInputAxisFn>> m_AxisCallbacks;
			// Holds all action mapping profiles.
			std::vector<ActionMapping> m_ActionMappings;
			// Holds all callback funcitons and their corisponding hints found the actoin mappings stored in InputDispatcher::m_ActionMappings.
			std::map<std::pair<std::string, InputEventType>, std::vector<EventInputActionFn>> m_ActionCallbacks;

		private:
			// Static instance of the Input Dispatcher.
			static InputDispatcher* s_Instance;
		};
	}
}