// Copyright Insight Interactive. All Rights Reserved.
#pragma once

#include "Runtime/Core/Public/ieObject/iePawn.h"

#include "Runtime/Core/Public/ieObject/Components/ieCameraComponent.h"


namespace Insight
{
	class INSIGHT_API iePlayerCharacter : public iePawn
	{
	public:
		iePlayerCharacter(ieWorld* pWorld)
			: iePawn(pWorld)
		{
			ieCameraComponent* Camera = CreateDefaultSubObject<ieCameraComponent>(TEXT("Player camera"));
			m_pCamera = Camera;

			//GetWorld()->SetSceneRenderCamera(m_pCamera);
			//m_pCamera->GetTransform().SetPosition(0.f, 0.f, -3.f);
			m_pCamera->GetTransform().SetParent(&m_Transform);
			m_CanRotateCamera = false;
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
			Controller.BindAxis("MoveForward", this, &iePawn::MoveForward);
			Controller.BindAxis("MoveRight", this, &iePawn::MoveRight);
			Controller.BindAxis("MoveUp", this, &iePawn::MoveUp);
			Controller.BindAction("Sprint", IET_Pressed, this, &iePawn::Sprint);
			Controller.BindAction("Sprint", IET_Released, this, &iePawn::Sprint);
			Controller.BindAxis("MouseWheelUp", (iePawn*)this, &iePawn::MoveForward);

			// Camera
			Controller.BindAction("CameraPitchYawLock", IET_Pressed, this, &iePlayerCharacter::TogglePitchYawRotation);
			Controller.BindAction("CameraPitchYawLock", IET_Released, this, &iePlayerCharacter::TogglePitchYawRotation);
			Controller.BindAxis("LookUp", this, &iePlayerCharacter::LookUp);
			Controller.BindAxis("LookRight", this, &iePlayerCharacter::LookRight);
		}

	protected:

		void LookUp(float Value)
		{
			if(m_CanRotateCamera)
				m_pCamera->GetTransform().Rotate(Value * m_CameraPitchSpeedMultiplier * GetWorld()->GetDeltaTime(), 0.0f, 0.0f);
		}

		void LookRight(float Value)
		{
			if (m_CanRotateCamera)
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
