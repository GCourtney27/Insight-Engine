#pragma once

#include "Runtime/Core/Public/ECS/Core/Core.h"



namespace ECS
{

	/*
		An Entity in the EntityAdmin.
	*/
	typedef unsigned long Entity_t;

	/*
		A invalid Entity instance.
	*/
	static const Entity_t InvalidEntity = -1;
	
	/*
		Returns true if the Entity is valid, false if not.
	*/
	ECS_FORCE_INLINE bool IsValidEntity(const Entity_t& Entity)
	{
		return Entity != InvalidEntity;
	}

	/*
		Invalidates an Entity. WARNING: does not release component references.
	*/
	ECS_FORCE_INLINE void InvalidateEntity(Entity_t& Entity)
	{
		Entity = InvalidEntity;
	}
}
