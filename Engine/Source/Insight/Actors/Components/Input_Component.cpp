#include "Engine_pch.h"

#include "Input_Component.h"

namespace Insight {
	
	namespace Runtime {


		InputComponent::InputComponent(AActor* pOwner)
			: ActorComponent("Input Component", pOwner)
		{
		}

		InputComponent::~InputComponent()
		{
		}

		void InputComponent::OnInit()
		{
		}

		void InputComponent::OnDestroy()
		{
		}

		void InputComponent::OnRender()
		{
		}

		void InputComponent::BeginPlay()
		{
		}

		void InputComponent::EditorEndPlay()
		{
		}

		void InputComponent::Tick(const float DeltaMs)
		{
		}

		void InputComponent::OnAttach()
		{
		}

		void InputComponent::OnDetach()
		{
		}

		void InputComponent::BindAction(const char* ActionName, InputEventType EventType, EventInputActionFn Callback)
		{
			InputDispatcher::Get().RegisterActionCallback(ActionName, EventType, Callback);
		}

		void InputComponent::BindAxis(const char* AxisName, EventInputAxisFn Callback)
		{
			InputDispatcher::Get().RegisterAxisCallback(AxisName, Callback);
		}

	}
}
