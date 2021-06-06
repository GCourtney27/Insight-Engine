#pragma once

#include "Core/Public/ECS/Core/Core.h"

#include "Core/Public/ECS/Component/ComponentUtil.h"
#include "Core/Public/ECS/DataStructures/ComponentMap.h"

#include <functional>



namespace ECS {

	/*
		Data Layout diagram
		-------------------
	
											 m_EntityAdminComponents [Map]
														|
														v
		(Key) ComponentHash [ComponentHash_t]			-			(Value) m_Components [Map]
																				  |
																				  v
															(Key) ComponentId [ComponentUID_t] - (Value) PackedKey [Owner [Entity_t] & Index [ArrayIndex]]
																																			|
																																			v
																															RawComponentData [Vector<UserDefinedType>]
	*/

	namespace EntityAdminHelpers
	{
		/*
			Returns a pointer to the component map in the EntityAdmin.

			@param (TemplateParam) ComponentMapType - The componenent type the map to retrieve holds.
			@param pBaseMap - A pointer to a 'EntityAdmin::m_EntityAdminComponents' value.
		*/
		template <typename ComponentMapType>
		ECS_FORCEINLINE constexpr auto TryGetComponentMapByBase(ComponentMapBase* pBaseMap)
		{
			return dynamic_cast<GenericComponentMap<ComponentMapType>*>(pBaseMap);
		}
	}


	class EntityAdmin
	{
	protected:
		/*
			The container for all the components in the EntityAdmin.
		*/
		std::unordered_map<ComponentHash_t, ComponentMapBase*> m_EntityAdminComponents;

		/*
			Incrementor that tracks the next available Entity ids that can be used.
		*/
		uint32_t m_AvailableEntityIndex;

	public:
		EntityAdmin()
			: m_AvailableEntityIndex(0)
		{
		}
		~EntityAdmin()
		{
			DebugLog("Cleaning Up Entity Admin.\n");

			Flush();
		}

		/*
			Destroy the EntityAdmin and all component maps and entities.
		*/
		void Flush()
		{
			for (auto& [Id, Map] : m_EntityAdminComponents)
				delete Map;

			m_AvailableEntityIndex = 0;
			m_EntityAdminComponents.clear();
		}

		template <typename ComponentType>
		auto GetComponentMap() const 
		{
			ComponentHash_t ComponentHash = ECS::ComponentHash<ComponentType>();
			auto Iter = m_EntityAdminComponents.find(ComponentHash);
			if (Iter != m_EntityAdminComponents.end())
				return EntityAdminHelpers::TryGetComponentMapByBase<ComponentType>(Iter->second);
			else
				return static_cast<GenericComponentMap<ComponentType>*>(nullptr);
		}

		/*
			Adds a component to the EntityAdmin and returns a pointer to the new instance. 
			Returns nullptr if creation fails.
			Warning: Return value is not to be cached! Only use the return pointer for basic initialization prceeding this call.

			@param Entity - The Entity to attach the component too.
			@param Args - Arguments for the componnets constructor.
		*/
		template <typename ComponentType, typename ... InitArgs>
		ComponentType& AddComponent(Entity_t Entity, InitArgs & ... Args)
		{
			// Verify input values.
			ValidateComponent<ComponentType>();
			RuntimeAssert(IsValidEntity(Entity)); // Trying to get a component for an invalid Entity.

			using SpecializedMap = GenericComponentMap<ComponentType>;
			// Fetch the hashed id of the component type.
			ComponentHash_t ComponentHash = ECS::ComponentHash<ComponentType>();
			ECS::ComponentMapBase* pMap = nullptr;

			// Locate the hashed component id in the EntityAdmin.
			auto Iter = m_EntityAdminComponents.find(ComponentHash);
			if (Iter != m_EntityAdminComponents.end())
			{
				pMap = Iter->second;
			}
			else
			{
				m_EntityAdminComponents[ComponentHash] = new SpecializedMap();
				pMap = m_EntityAdminComponents[ComponentHash];
			}

			auto ComponentMap = EntityAdminHelpers::TryGetComponentMapByBase<ComponentType>(pMap);
			return ComponentMap->AddComponent(Entity, Args...);
		}

		/*
			Gets a pointer to the component from the EntityAdmin with its uniue ID.
			Returns nullptr if no component exists for it.
			@param ComponentId - The unique id of the component to get.
		*/
		template <typename ComponentExecutionType>
		ECS_NO_DISCARD ComponentExecutionType* GetComponentById(const ComponentUID_t& ComponentId)
		{
			// Verify input values.
			ValidateComponent<ComponentExecutionType>();
			
			using SpecializedMap = GenericComponentMap<ComponentExecutionType>;
			// Fetch the hashed id of the component type.
			ComponentHash_t ComponentHash = ECS::ComponentHash<ComponentExecutionType>();

			// Locate the component type in the EntityAdmin.
			auto Iter = m_EntityAdminComponents.find(ComponentHash);
			if (Iter != m_EntityAdminComponents.end())
			{
				// Find the Entity that owns the component of that type.
				auto ComponentMap = EntityAdminHelpers::TryGetComponentMapByBase<ComponentExecutionType>(Iter->second);
				return ComponentMap->GetComponentById(ComponentId);
			}

			return static_cast<ComponentExecutionType*>(nullptr);
		}

		/*
			Remove a component from an Entity by its unique ID.
			@param ComponentId - The unique id of the component to delete.
		*/
		template <typename ComponentExecutionType>
		void RemoveComponentById(const ComponentUID_t& ComponentId)
		{
			// Verify input values.
			ValidateComponent<ComponentExecutionType>();
			
			// Fetch the hashed id of the component type.
			ComponentHash_t ComponentHash = ECS::ComponentHash<ComponentExecutionType>();

			// Find the component in the EntityAdmin.
			auto Iter = m_EntityAdminComponents.find(ComponentHash);
			if (Iter != m_EntityAdminComponents.end())
			{
				// Ge the map and find the Entity holding the instances of the components.
				auto ComponentMap = EntityAdminHelpers::TryGetComponentMapByBase<ComponentExecutionType>(Iter->second);
				ComponentMap->RemoveComponentById(ComponentId);
			}
		}

		/*
			Destroy an Entity and its associated components from the EntityAdmin.
			@param Entity - The entity to destroy.
		*/
		void DestroyEntity(Entity_t& Entity)
		{
			if (!IsValidEntity(Entity)) // Trying to get a component for an invalid Entity.
				return;

			// Remove all the componets associated with the Entity.
			for (auto& [Id, Map] : m_EntityAdminComponents)
				Map->DestroyEntityRefs(Entity);

			InvalidateEntity(Entity);
		}

		/*
			Creates an Entity and returns it. If creation fails, an invalid Entity is returned.
		*/
		ECS_NO_DISCARD Entity_t CreateEntity()
		{
			Entity_t Entity = static_cast<Entity_t>(m_AvailableEntityIndex++);
			if (IsValidEntity(Entity))
				return Entity;
			else
				return InvalidEntity;
		}

	};
}
