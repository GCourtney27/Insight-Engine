#pragma once

#include "ECS/Core/Core.h"

#include <assert.h>



namespace ECS
{

	class SystemBase
	{
	protected:
		const char* m_DebugName;

	protected:
		SystemBase(const char* DebugName)
			: m_DebugName(DebugName)
		{
		}
		virtual ~SystemBase() = default;

	public:
		ECS_FORCE_INLINE const char* GetDebugName() const
		{
			return m_DebugName;
		}
	};


	template <typename ComponentType>
	class GenericSystem : public SystemBase
	{
	protected:
		GenericComponentMap<ComponentType>& m_ComponentMapRef;

	public:
		GenericSystem(const EntityAdmin& EntityAdmin, const char* DebugName = "")
			: SystemBase(DebugName)
			, m_ComponentMapRef(*EntityAdmin.GetComponentMap<ComponentType>())
		{
		}
		virtual ~GenericSystem() {}

		/*
			Executes the system for all components.
		*/
		virtual void Execute() = 0;

		/*
			Returns a reference to the container holding the raw component data.
		*/
		ECS_FORCE_INLINE std::vector<ComponentType>& GetRawComponentData()
		{
			return m_ComponentMapRef.m_RawComponents;
		}
	};

}
