#pragma once
#include <Runtime/Core.h>

#include "Runtime/Events/Event.h"


namespace Insight {


#define RETURN_IF_COMPONENT_DISABLED if(!m_Enabled){return;}

	namespace GameFramework {


		class ActorComponent
		{
		public:
			using EventCallbackFn = std::function<void(Event&)>;

		public:
			virtual ~ActorComponent(void) { m_pOwner = nullptr; }

			virtual bool LoadFromJson(const rapidjson::Value& JsonComponent) = 0;
			virtual bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer) = 0;

			virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
			virtual void OnEvent(Event& e) = 0;

			virtual void OnInit() = 0;
			virtual void OnPostInit() {}
			virtual void OnDestroy() = 0;
			virtual void OnRender() = 0;
			virtual void OnUpdate(const float DeltaTime) {}
			virtual void OnChanged() {}
			// Sub-menus this component sould render when its owning actor is 
			// selected in the details panel. 
			// NOTE: This method should always 
			// push and pop a ID in imgui to prevent dupllicated component types
			// from editing eachother inadvertently.
			virtual void OnImGuiRender() = 0;
			virtual void RenderSceneHeirarchy() = 0;

			virtual void BeginPlay() = 0;
			virtual void EditorEndPlay() = 0;
			virtual void Tick(const float DeltaMs) = 0;

			virtual void OnAttach() = 0;
			virtual void OnDetach() = 0;

			//void SetEventCallbackFunction()
			bool GetIsComponentEnabled() const { return m_Enabled; }
			void SetComponentEnabled(bool Enable) { m_Enabled = Enable; }

			const char* GetName() const { return m_ComponentName; };

			void SetOwner(AActor* Owner) { m_pOwner = Owner; }
		protected:
			ActorComponent(const char* ComponentName, GameFramework::AActor* Owner)
				: m_ComponentName(ComponentName), m_pOwner(Owner) 
			{}
		protected:
			GameFramework::AActor* m_pOwner;
			const char* m_ComponentName;
			bool m_Enabled = true;

			// ImGui uses ID's to sort through subwindows.
			// If we dont add a uid to each submenu, components of 
			// the same type will get edited at the same time.
			char m_IDBuffer[30];
		};
	} // end namespace GameFramework
} // end namespace Insight
