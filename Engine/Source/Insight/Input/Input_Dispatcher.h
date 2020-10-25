#pragma once

#include <Insight/Core.h>

#include "Insight/Events/Event.h"
#include "Insight/Events/Key_Event.h"
#include "Insight/Events/Mouse_Event.h"

namespace Insight {



		using EventInputAxisFn = std::function<void(float)>;
		using EventInputActionFn = std::function<void(void)>;

		struct AxisMapping
		{
			const char Hint[32];
			KeymapCode MappedKeycode;
			float Scale;
		};
		
		struct ActionMapping
		{
			const char Hint[32];
			KeymapCode MappedKeycode;
			bool CanDispatch = true;
		};

		class INSIGHT_API InputDispatcher
		{
		public:
			InputDispatcher()
			{
				s_Instance = this;
				
				// TODO load these from a settings file
				// TODO Change axis mappings depending on play mode (Editor/InGame)
				m_AxisMappings.push_back({ "MoveForward", KeymapCode_Keyboard_W, 1.0f });
				m_AxisMappings.push_back({ "MoveForward", KeymapCode_Keyboard_S, -1.0f });
				m_AxisMappings.push_back({ "MoveRight", KeymapCode_Keyboard_D, 1.0f });
				m_AxisMappings.push_back({ "MoveRight", KeymapCode_Keyboard_A, -1.0f });
				m_AxisMappings.push_back({ "MoveUp", KeymapCode_Keyboard_E, 1.0f });
				m_AxisMappings.push_back({ "MoveUp", KeymapCode_Keyboard_Q, -1.0f });

				m_AxisMappings.push_back({ "LookUp", KeymapCode_Mouse_MoveY, 1.0f });
				m_AxisMappings.push_back({ "LookUp", KeymapCode_Mouse_MoveY, -1.0f });
				m_AxisMappings.push_back({ "LookRight", KeymapCode_Mouse_MoveX, 1.0f });
				m_AxisMappings.push_back({ "LookRight", KeymapCode_Mouse_MoveX, -1.0f });

				//m_ActionMappings.push_back({ "ButtonPress", KeymapCode_Mouse_Button_Right });
				m_ActionMappings.push_back({ "ButtonPress", KeymapCode_Keyboard_K });
				m_ActionMappings.push_back({ "MouseButtonPress", KeymapCode_Mouse_Button_Right });

			}
			~InputDispatcher() = default;
			static InputDispatcher& Get() { return *s_Instance; }

			void UpdateInputs();
			void ProcessInputEvent(Event& e);
			
			void RegisterAxisCallback(const char* Name, EventInputAxisFn Callback);
			void RegisterActionCallback(const char* Name, InputEventType EventType, EventInputActionFn Callback);


		private:
			bool DispatchKeyPressEvent(KeyPressedEvent& e);
			bool DispatchMouseMoveEvent(MouseMovedEvent& e);
			bool DispatchActionEvent(InputEvent& e);

		private:

			std::vector<AxisMapping> m_AxisMappings;
			std::map<std::string, std::vector<EventInputAxisFn>> m_AxisCallbacks;

			std::vector<ActionMapping> m_ActionMappings;
			std::map<std::pair<std::string, InputEventType>, std::vector<EventInputActionFn>> m_ActionCallbacks;

		private:
			static InputDispatcher* s_Instance;
		};

}