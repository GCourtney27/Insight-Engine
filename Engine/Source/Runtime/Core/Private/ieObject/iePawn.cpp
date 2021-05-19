#include <Engine_pch.h>

#include "Runtime/Core/Public/ieObject/iePawn.h"

#include "Runtime/Core/Public/ieObject/ieWorld.h"


namespace Insight
{
	void iePawn::Move(const FVector3& Direction, const float Value)
	{
		float Velocity = m_MovementSpeed * Value * GetWorld()->GetDeltaTime();
		FVector3 Pos = m_Transform.GetPosition();
		Pos += Direction * Velocity;
		m_Transform.SetPosition(Pos);
	}

	void iePawn::MoveForward(float Value)
	{
		Move(m_Transform.GetLocalForward(), Value);
	}

	void iePawn::MoveRight(float Value)
	{
		Move(m_Transform.GetLocalRight(), Value);
	}

	void iePawn::MoveUp(float Value)
	{
		Move(m_Transform.GetLocalUp(), Value);
	}

	void iePawn::Sprint()
	{
		m_bIsSprinting = !m_bIsSprinting;
		if (m_bIsSprinting)
		{
			m_CurrentModementSpeed = m_MovementSpeed;
		}
		else
		{
			m_CurrentModementSpeed = m_SprintSpeed;
		}
	}
}
