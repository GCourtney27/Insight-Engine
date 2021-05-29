// Copyright Insight Interactive. All Rights Reserved.
#pragma once

#include <Runtime/Core.h>

#include "Runtime/Core/Public/ieObject/iePawn.h"
#include "Runtime/Core/Public/ieObject/Components/ieCameraComponent.h"
#include "Runtime/Core/Public/ieObject/ieWorld.h"

namespace Insight
{

	class INSIGHT_API ieCameraActor : public iePawn
	{
	public:
		ieCameraActor(ieWorld* pWorld)
			: iePawn(pWorld)
			, m_CanRotateCamera(false)
		{
			m_pCamera = CreateDefaultSubObject<ieCameraComponent>(TEXT("Player camera"));

			m_pCamera->GetTransform().SetParent(&m_Transform);
		}
		virtual ~ieCameraActor()
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
			Controller.BindAxis("MoveForward", this, &iePawn::MoveForward);
			Controller.BindAxis("MoveRight", this, &iePawn::MoveRight);
			Controller.BindAxis("MoveUp", this, &iePawn::MoveUp);
			Controller.BindAxis("MouseWheelUp", this, &iePawn::MoveForward);
			Controller.BindAction("Sprint", IET_Pressed, this, &iePawn::Sprint);
			Controller.BindAction("Sprint", IET_Released, this, &iePawn::Sprint);

			// Camera
			Controller.BindAction("CameraPitchYawLock", IET_Pressed, this, &ieCameraActor::TogglePitchYawRotation);
			Controller.BindAction("CameraPitchYawLock", IET_Released, this, &ieCameraActor::TogglePitchYawRotation);
			Controller.BindAxis("LookUp", this, &ieCameraActor::LookUp);
			Controller.BindAxis("LookRight", this, &ieCameraActor::LookRight);
		}

		ieCameraComponent* GetCamera() const
		{
			return m_pCamera;
		}

	protected:

		void LookUp(float Value)
		{
			if (m_CanRotateCamera)
			{
				float LookAngle = FVector3::Forward.Dot(m_pCamera->GetTransform().GetLocalForward());
				//if (LookAngle > 1.f)
				{
					m_Transform.Rotate(Value * m_CameraPitchSpeedMultiplier * GetWorld()->GetDeltaTime(), 0.0f, 0.0f);
					m_pCamera->GetTransform().SetRotation(m_Transform.GetRotation());
				}
			}
		}

		void LookRight(float Value)
		{
			if (m_CanRotateCamera)
			{
				m_Transform.Rotate(0.0f, Value * m_CameraYawSpeedMultiplier * GetWorld()->GetDeltaTime(), 0.0f);
				m_pCamera->GetTransform().SetRotation(m_Transform.GetRotation());
			}
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
