#pragma once
#include <Insight/Core.h>

#include "Insight/Runtime/Identification/Object.h"

namespace Insight {

	class AActor;

	class Component : public Object
	{
	public:
		virtual void Destroy() = 0;
		virtual void BeginPlay() = 0;
		virtual void Tick(const float& deltaTime) = 0;
		virtual void OnImGuiRender() = 0;

		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;

		bool& GetIsComponentEnabled() { return m_Enabled; }
		void SetComponentEnabled(bool enable) { m_Enabled = enable; }

		AActor* GetOwner() { return m_Owner; }

		void SetName(const char* name) { m_Name = name; }
		const char* GetName() { return m_Name; }
	protected:
		Component(AActor* owner, const ID& id = ID())
			: m_Owner(owner), Object(id) {}
	protected:
		AActor* m_Owner = nullptr;
		const char* m_Name = nullptr;
		bool m_Enabled = true;


	};
}

