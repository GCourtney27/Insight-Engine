#pragma once

#include <Runtime/Core.h>

#include "Runtime/Core/Public/ieObject/ieActor.h"
#include "Runtime/Core/Public/ieObject/Components/ieControllerComponent.h"

namespace Insight
{
	static const float kDefaultMovementSpeed = 60.f;
	static const float kDefaultSprintSpeed = kDefaultMovementSpeed * 2.f;
	static const float kDefaultCameraPitchSpeedMultiplier = 5.f;
	static const float kDefaultCameraYawSpeedMultiplier = 5.f;

	class INSIGHT_API iePawn : public ieActor
	{
	public:
		using Super = iePawn;

		iePawn(ieWorld* pWorld)
			: ieActor(pWorld, L"Default Pawn")
			, m_CameraPitchSpeedMultiplier(kDefaultCameraPitchSpeedMultiplier)
			, m_CameraYawSpeedMultiplier(kDefaultCameraYawSpeedMultiplier)
			, m_MovementSpeed(kDefaultMovementSpeed)
			, m_SprintSpeed(kDefaultSprintSpeed)
			, m_bIsSprinting(false)
			, m_pController(NULL)
			, m_Transform()
			, m_CurrentModementSpeed(0.f)
		{
			ieControllerComponent& Controller = CreateDefaultSubObject<ieControllerComponent>(TEXT("Player Controller"));
			m_pController = &Controller;
		}
		virtual ~iePawn()
		{
		}

		virtual void BeginPlay() override
		{
			SetupController(*m_pController);
		}

		void MoveForward(float Value);
		void MoveRight(float Value);
		void MoveUp(float Value);
		void Sprint();

	protected:
		virtual void SetupController(ieControllerComponent& Controller)
		{
			// Override for behavior.
		}


	private:
		void Move(const FVector3& Direction, const float Value);


	protected:
		float m_CameraPitchSpeedMultiplier;
		float m_CameraYawSpeedMultiplier;
		float m_MovementSpeed;
		float m_SprintSpeed;
		bool m_bIsSprinting;
		ieControllerComponent* m_pController;
		ieTransform m_Transform;


	private:
		float m_CurrentModementSpeed;

	};
}
