// Copyright Insight Interactive. All Rights Reserved.
#pragma once

#include "Runtime/Core/Public/ieObject/iePawn.h"

#include "Runtime/Core/Public/ieObject/Components/ieCameraComponent.h"


namespace Insight
{
	class INSIGHT_API iePlayerCharacter : iePawn
	{
	public:
		iePlayerCharacter(ieWorld* pWorld)
			: iePawn(pWorld)
		{
			ieCameraComponent& Camera = CreateDefaultSubObject<ieCameraComponent>(TEXT("Player camera"));
			m_pCamera = &Camera;

			GetWorld()->SetSceneRenderCamera(m_pCamera);
			m_pCamera->GetTransform().SetPosition(0.f, 0.f, -3.f);
			m_pCamera->GetTransform().SetParent(&m_Transform);
		}
		virtual ~iePlayerCharacter()
		{
		}

		virtual void BeginPlay() override
		{
			Super::BeginPlay();
		}

		virtual void Tick(float DeltaMs) override
		{
			Super::Tick(DeltaMs);
		}

		virtual void SetupController(ieControllerComponent& Controller) override
		{
			Super::SetupController(Controller);
			// Setup event callbacks for camera movement.
			//
			// Locamotion
			Controller.BindAxis("MoveForward", IE_BIND_EVENT_FN(iePawn::MoveForward, (iePawn*)this));
			Controller.BindAxis("MoveRight", IE_BIND_EVENT_FN(iePawn::MoveRight, (iePawn*)this));
			Controller.BindAxis("MoveUp", IE_BIND_EVENT_FN(iePawn::MoveUp, (iePawn*)this));
			Controller.BindAxis("MouseWheelUp", IE_BIND_EVENT_FN(iePawn::MoveForward, (iePawn*)this));
			Controller.BindAction("Sprint", IET_Pressed, std::bind(&iePawn::Sprint, (iePawn*)this));
			Controller.BindAction("Sprint", IET_Released, std::bind(&iePawn::Sprint, (iePawn*)this));

			// Camera
			Controller.BindAction("CameraPitchYawLock", IET_Pressed, std::bind(&iePlayerCharacter::TogglePitchYawRotation, this));
			Controller.BindAction("CameraPitchYawLock", IET_Released, std::bind(&iePlayerCharacter::TogglePitchYawRotation, this));
			Controller.BindAxis("LookUp", IE_BIND_EVENT_FN(iePlayerCharacter::LookUp, this));
			Controller.BindAxis("LookRight", IE_BIND_EVENT_FN(iePlayerCharacter::LookRight, this));
		}

	protected:

		void LookUp(float Value)
		{
			m_pCamera->GetTransform().Rotate(Value * m_CameraPitchSpeedMultiplier * GetWorld()->GetDeltaTime(), 0.0f, 0.0f);
		}

		void LookRight(float Value)
		{
			m_pCamera->GetTransform().Rotate(0.0f, Value * m_CameraYawSpeedMultiplier * GetWorld()->GetDeltaTime(), 0.0f);
		}

		void TogglePitchYawRotation()
		{
			m_CanRotateCamera = !m_CanRotateCamera;
		}

	protected:
		ieCameraComponent* m_pCamera;

	private:
		bool m_CanRotateCamera;
	};
}
