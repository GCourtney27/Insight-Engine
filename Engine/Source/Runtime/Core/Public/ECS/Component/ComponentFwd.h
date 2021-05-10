#pragma once

#include "ECS/Core/Core.h"
#include "ECS/EntityAdmin/EntityFwd.h"

#include <string_view>



namespace ECS 
{

	/*
		Hashed identifier for component type. Unique
		for each component, but not each instance.
	*/
	typedef unsigned long long ComponentHash_t;

	/*
		Unique identifier for each component instance.
	*/
	typedef unsigned long long ComponentUID_t;

	/*
		Base class for all components that wish to be used by the code ECS system.
		Uses the curiously recurring template (CRT) pattern https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
		where the derived class is the template parameter for the base class.
	*/
	template <typename ComponentType>
	struct ComponentBase
	{
	protected:
		ComponentUID_t m_UID;
		Entity_t m_Owner;

	public:

		/*
			Returns the components unique ID.
		*/
		ECS_FORCE_INLINE ComponentUID_t GetId() const noexcept(true) { return m_UID; }

		/*
			Set the components unique ID.
		*/
		ECS_FORCE_INLINE void SetId(const ComponentUID_t& ID) noexcept(true) { m_UID = ID; }


		ECS_FORCE_INLINE bool operator ==(const ComponentType& rhs)
		{
			return this->GetId() == rhs.GetId();
		}

		ECS_FORCE_INLINE bool operator !=(const ComponentType& rhs)
		{
			return !(this == rhs);
		}

	protected:
		ComponentBase()
		{
			// Zero the component.
			ZeroMemRanged(this, sizeof(ComponentBase<ComponentType>));

			// Register the ID.
			RegisterNewId(m_UID);
		}
		virtual ~ComponentBase() = default;

	private:
		/*
			Registers a new it for this component type.
			@param OutId - The Id to register.
		*/
		ECS_FORCE_INLINE static void RegisterNewId(ComponentUID_t& OutId)
		{
			OutId = s_ComponentID++;
		}

	private:
		/*
			Static Id counter for all components of this type.
		*/
		static ComponentUID_t s_ComponentID;
	};

	template <typename ComponentType>
	ComponentUID_t ComponentBase<ComponentType>::s_ComponentID = 0;

}