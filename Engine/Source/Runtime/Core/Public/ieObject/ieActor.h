// Copyright Insight Interactive. All Rights Reserved.
#pragma once

#include "EngineDefines.h"

#include "Core/Public/ieObject/ieObjectBase.h"

#include "Core/Public/ieObject/Components/ieComponentBase.h"

namespace Insight
{
	class ieWorld;
	namespace Graphics
	{
		class ICommandContext;
	}

	class INSIGHT_API ieActor : public ieObjectBase
	{
	public:
		using Super = ieActor;

		ieActor(ieWorld* pWorld, const FString& Name = L"Unamed ieActor");
		virtual ~ieActor()
		{
		}
		
		virtual void BeginPlay()
		{
			for (UInt32 i = 0; i < m_NumSubobjects; ++i)
			{
				m_SubObjects[i]->BeginPlay();
			}
		}

		virtual void Tick(float DeltaMs)
		{
			// TODO: This makes me want to puke... fix the ecs
			//		The cache is being shredded here.
			for (UInt32 i = 0; i < m_NumSubobjects; ++i)
			{
				m_SubObjects[i]->Tick(DeltaMs);
			}
		}

		virtual void Render(Graphics::ICommandContext& GfxContext)
		{
			for (UInt32 i = 0; i < m_NumSubobjects; ++i)
			{
				m_SubObjects[i]->Render(GfxContext);
			}
		}

		inline bool CanReceiveTickEvents() const;

		inline bool CanBeTicked(bool Value);


		template <typename SubObjectType, typename ... Args>
		FORCEINLINE SubObjectType* CreateDefaultSubObject(const FString& Name, Args ... args)
		{
#if IE_CACHEOPTIMIZED_ECS_ENABLED
			SubObjectType& pResult = GetWorld()->GetEntityAdmin().AddComponent<SubObjectType>(GetWorldId(), args...);
			PushComponentId(pResult.GetId());
			pResult.SetOwner(this);
			return pResult;
#else
			ieComponentBase* pComponent = new SubObjectType(this);
			IE_ASSERT(pComponent, "Trying to add null component to actor.");

			pComponent->OnCreate();
			pComponent->OnAttach();

			m_SubObjects.push_back(pComponent);
			m_NumSubobjects++;
			return SCast<SubObjectType*>(pComponent);
#endif
			
		}

		template<typename SubObjectType>
		SubObjectType* GetSubobject()
		{
			SubObjectType* pComponent = nullptr;
			for (ieComponentBase* _component : m_SubObjects)
			{
				pComponent = DCast<SubObjectType*>(_component);
				if (pComponent != nullptr) break;
			}
			return pComponent;
		}

		template <typename SubObjectType>
		FORCEINLINE void RemoveSubObjectById(const ECS::ComponentUID_t& ComponentId)
		{
#if IE_CACHEOPTIMIZED_ECS_ENABLED
			GetWorld()->GetEntityAdmin().RemoveComponentById<SubObjectType>(ComponentId);
#endif
		}

		template <typename SubObjectType>
		FORCEINLINE void RemoveSubObject(const SubObjectType& Component)
		{
#if IE_CACHEOPTIMIZED_ECS_ENABLED
			GetWorld()->GetEntityAdmin().RemoveComponentById<SubObjectType>(Component.GetId());
#else

			auto iter = std::find(m_Components.begin(), m_Components.end(), &Component);
			(*iter)->OnDetach();
			(*iter)->OnDestroy();
			m_SubObjects.erase(iter);
			m_NumSubobjects--;
#endif
		}

	protected:
		bool m_bCanReveiveTickEvents;
	};


	//
	// Inline function implementations
	//

	inline bool ieActor::CanReceiveTickEvents() const 
	{
		return m_bCanReveiveTickEvents;
	}

	inline bool ieActor::CanBeTicked(bool Value)
	{
		m_bCanReveiveTickEvents = Value;
	}

}
