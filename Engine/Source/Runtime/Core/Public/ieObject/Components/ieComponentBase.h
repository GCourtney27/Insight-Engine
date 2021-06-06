#pragma once

#include "EngineDefines.h"

#include "Core/Public/ieObject/ieObjectBase.h"

namespace Insight
{
	namespace Graphics
	{
		class ICommandContext;
	}

	//template <typename DerivedType>
	class INSIGHT_API ieComponentBase/* : public ECS::ComponentBase<DerivedType>*/
	{
		friend class ieActor;
		friend class ieObjectBase;
	public:
		using Super = ieComponentBase;

	protected:
		ieComponentBase()
			: m_pOwner(NULL)
		{
		}
		virtual ~ieComponentBase()
		{
		}

		virtual void BeginPlay()
		{
		}

		virtual void Tick(float DeltaMs)
		{
		}

		virtual void Render(Graphics::ICommandContext& GfxContext)
		{
		}

		virtual void OnAttach()
		{
		}

		virtual void OnDetach()
		{
		}

		virtual void OnCreate()
		{
		}

		virtual void OnDestroy()
		{
		}

		inline void SetOwner(ieObjectBase* pOwner)
		{
			m_pOwner = pOwner;
		}

		inline const FString& GetName() const
		{
			return m_Name;
		}

		inline void SetName(const FString& Name)
		{
			m_Name = Name;
		}

	private:
		ieObjectBase* m_pOwner;
		FString m_Name;
	};
}
