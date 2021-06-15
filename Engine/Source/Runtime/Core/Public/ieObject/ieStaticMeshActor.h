// Copyright Insight Interactive. All Rights Reserved.
#pragma once

#include "EngineDefines.h"

#include "Core/Public/ieObject/ieActor.h"
#include "Core/Public/ieObject/Components/ieStaticMeshComponent.h"
#include "Core/Public/ieObject/Components/iePointLightComponent.h"
#include "Core/Public/ieObject/Components/ieDirectionalLightComponent.h"

namespace Insight
{
	class INSIGHT_API ieStaticMeshActor : public ieActor
	{
	public:
		ieStaticMeshActor(ieWorld* pWorld)
			: ieActor(pWorld, L"ieStaticMeshActor")
		{
			ieStaticMeshComponent* pMesh = CreateDefaultSubObject<ieStaticMeshComponent>(TEXT("Static mesh component"));
			pMesh->GetTransform().SetParent(&m_Transform);

			iePointLightComponent* pLight = CreateDefaultSubObject<iePointLightComponent>(TEXT(""));
			pLight->SetColor(1.f, 1.f, 1.f);

			ieDirectionalLightComponent* pDirLight = CreateDefaultSubObject<ieDirectionalLightComponent>(TEXT(""));
			pDirLight->SetColor(0.3, 0.3, 0.f);
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
