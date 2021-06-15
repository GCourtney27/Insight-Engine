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

		inline const FString& GetName() const
		{
			return m_Name;
		}

		inline void SetName(const FString& Name)
		{
			m_Name = Name;
		}

	private:
		FString m_Name;
	};
}
