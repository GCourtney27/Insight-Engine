#pragma once

#include "Runtime/Core/Public/ECS/Core/Core.h"
#include "Runtime/Core/Public/ECS/Component/ComponentFwd.h"

#include <string_view>



namespace ECS 
{

	/*
		Compile-time check to ensure a component is of a type the core ECS can understand.
	*/
	template <class TargetComponent>
	constexpr void ValidateComponent()
	{
		constexpr bool IsValid = std::is_base_of<ComponentBase<TargetComponent>, TargetComponent>::value;
		static_assert(IsValid, "Target component is not a valid component. It does not derive from ComponentBase<T>");
	}

	/*
		Hashes a component into a faster lookup value.
	*/
	template <typename ComponentType>
	ECS_INLINE ComponentHash_t ComponentHash()
	{
		// Make sure the passed in object is a valid component.
		ValidateComponent<ComponentType>();
		
		// Hash the component name and return it.
		std::string_view Name = typeid(ComponentType).name();
		return static_cast<ComponentHash_t>(std::hash<std::string_view>()(Name));
	}
}
