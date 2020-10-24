#pragma once

#include <Insight/Core.h>

#include "Insight/Events/Event.h"
#include "Insight/Input/Key_Codes.h"
#include "Insight/Events/Key_Event.h"
#include "Insight/Events/Mouse_Event.h"

namespace Insight {



		using EventInputAxisFn = std::function<void(float)>;	// void MyFn(float);
		using EventInputActionFn = std::function<void(void)>;	// void MyFn(void);
		
		struct ActionBinding
		{
			const char* ActionName;
			InputEventType Type;
			EventInputActionFn ActionBindFn;
		};

		struct AxisBinding
		{
			const char* AxisName;
			EventInputAxisFn AxisBindFn;
		};

		struct AxisMapping
		{
			const char* Hint;
			uint32_t Keycode;
			float Scale;
		};
		
		struct ActionMapping
		{
			const char* Hint;
			uint32_t Keycode;
		};


		class INSIGHT_API InputDispatcher
		{
		public:
			InputDispatcher()
			{
				s_Instance = this;
				
				// TODO load these from a settings file
				//AxisMapping MoveForward{ "MoveForward", 'A', 1.0f };
				//m_AxisMappingsMap["MoveForward"] = MoveForward;

				m_AxisMappings.push_back({ "MoveForward", 'W', 1.0f });
				m_AxisMappings.push_back({ "MoveForward", 'S', -1.0f });
				m_AxisMappings.push_back({ "MoveRight", 'D', 1.0f });
				m_AxisMappings.push_back({ "MoveRight", 'A', -1.0f });

			}
			~InputDispatcher() = default;
			static InputDispatcher& Get() { return *s_Instance; }

			void ProcessInputEvent(Event& e);
			
			void RegisterAxisCallback(const char* Name, EventInputAxisFn Callback);
			void RegisterActionCallback(const char* Name, InputEventType EventType, EventInputActionFn Callback);


		private:
			bool DispatchKeyPressEvent(KeyPressedEvent& e);
			bool DispatchKeyReleaseEvent(KeyReleasedEvent& e);
			
		private:

			std::vector<AxisMapping> m_AxisMappings;
			std::vector<ActionMapping> m_ActionMappings;

			std::map<std::string, std::vector<EventInputAxisFn>> m_AxisCallbacks;

		private:
			static InputDispatcher* s_Instance;
		};

}