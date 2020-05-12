#pragma once
#include <Insight/Core.h>

#include "Insight/Utilities/Hashed_String.h"

namespace Insight {


	
	class ActorComponent
	{
	public:
		virtual ~ActorComponent(void) { m_pOwner.reset(); }

		virtual void OnInit() = 0;
		virtual void OnPostInit() {}
		virtual void OnDestroy() = 0;
		virtual void OnUpdate(const float& deltaTime) {}
		virtual void OnChanged() {}
		virtual void OnImGuiRender() = 0;

		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;

		const bool& GetIsComponentEnabled() const { return m_Enabled; }
		void SetComponentEnabled(bool enable) { m_Enabled = enable; }

		virtual ComponentId GetId() const { GetIdFromName(GetName()); }
		virtual const char* GetName() const = 0;
		static ComponentId GetIdFromName(const char* componentString)
		{
			void* rawId = HashedString::HashName(componentString);
			return reinterpret_cast<ComponentId>(rawId);
		}
	private:
		void SetOwner(StrongActorPtr owner) { m_pOwner = owner; }
	protected:
		StrongActorPtr m_pOwner;
		bool m_Enabled = true;

	};
}

