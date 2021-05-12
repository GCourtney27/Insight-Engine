// Copyright Insight Interactive. All Rights Reserved.
#include <Engine_pch.h>

#include "Runtime/Core/Public/ieObject/ieObjectBase.h"

#include "Runtime/Core/Public/ieObject/ieWorld.h"

namespace Insight
{

	ieObjectBase::ieObjectBase(class ieWorld* pWorld, const FString& Name)
		: m_pWorld(pWorld)
		, m_Name(Name)
	{
		IE_ASSERT(pWorld != NULL);
		AddToWorld();
	}

	ieObjectBase::~ieObjectBase()
	{
		RemoveFromWorld();
	}

	void ieObjectBase::AddToWorld()
	{
		m_WorldId = m_pWorld->CreateEntity();
	}

	void ieObjectBase::RemoveFromWorld()
	{
		// Remove the actor and destroy all of its components.
		m_pWorld->DestroyEntity(m_WorldId);
	}
}
