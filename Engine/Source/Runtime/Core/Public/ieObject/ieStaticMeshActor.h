// Copyright Insight Interactive. All Rights Reserved.
#pragma once

#include <Runtime/Core.h>

#include "Runtime/Core/Public/ieObject/ieActor.h"
#include "Runtime/Core/Public/ieObject/Components/ieStaticMeshComponent.h"

namespace Insight
{
	class INSIGHT_API ieStaticMeshActor : public ieActor
	{
	public:
		ieStaticMeshActor(ieWorld* pWorld)
			: ieActor(pWorld, L"ieStaticMeshActor")
		{
			ieStaticMeshComponent& pMesh = CreateDefaultSubObject<ieStaticMeshComponent>(TEXT("Static mesh component"));
			pMesh.GetTransform().SetParent(&m_Transform);
		}
		virtual ~ieStaticMeshActor()
		{
		}

		virtual void BeginPlay() override
		{
			Super::BeginPlay();
		}

		virtual void Tick(float DeltaMs) override
		{
			Super::Tick(DeltaMs);
			//m_Transform.Rotate(0.f, 0.001f, 0.f);
		}

		inline ieTransform& GetTransform()
		{
			return m_Transform;
		}

	private:
		ieTransform m_Transform;

	};

}
