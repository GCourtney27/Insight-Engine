#pragma once

#include "EngineDefines.h"
#include "Core/Public/ieObject/Components/ieComponentBase.h"

#include "Math/Public/Transform.h"



namespace Insight
{
	const float kDefaultFOV = 75.f;
	const float kDefaultNearZ = 0.001f;
	const float kDefaultFarZ = 1000.f;
	
	enum EViewType
	{
		VT_Perspective,
		VT_Orthographic,
	};

	struct ProjectionProperties
	{
		constexpr ProjectionProperties()
			: NearZ(0.01f)
			, FarZ(1000.f)
			, ViewMat(FMatrix::Identity)
			, ProjectionMat(FMatrix::Identity)
			, ProjectionType(VT_Perspective)
		{
		}
		float NearZ;
		float FarZ;
		FMatrix ViewMat;
		FMatrix ProjectionMat;
		EViewType ProjectionType;
	};

	class INSIGHT_API ieCameraComponent : public ieComponentBase/*<ieCameraComponent>*/
	{
	protected:
		friend class CameraSystem;

		ieTransform m_Transform;
		ProjectionProperties m_ViewProps;
		float m_FieldOfView;

	public:
		ieCameraComponent(ieActor* pOwner)
		{
			UpdateViewMat();
			
			SetProjectionValues(kDefaultFOV, 1600.f, 900.f, kDefaultNearZ, kDefaultFarZ);
		}
		virtual ~ieCameraComponent() 
		{
		}
		ieCameraComponent(ieCameraComponent&& Other) noexcept
		{
			m_Transform = std::move(Other.m_Transform);
			m_ViewProps = Other.m_ViewProps;
		}
		ieCameraComponent& operator=(const ieCameraComponent& Other) = default;

		virtual void Tick(float DeltaMs) override
		{
			UpdateViewMat();
		}

		inline ieTransform& GetTransform()
		{
			return m_Transform;
		}

		inline const FMatrix& GetViewMatrix()
		{
			return m_ViewProps.ViewMat;
		}

		inline const FMatrix& GetProjectionMatrix()
		{
			return m_ViewProps.ProjectionMat;
		}

		inline float GetNearZ()
		{
			return m_ViewProps.NearZ;
		}

		inline float GetFarZ()
		{
			return m_ViewProps.FarZ;
		}

		void SetProjectionValues(float FOVDegrees, float Width, float Height, float NearZ, float FarZ)
		{
			m_ViewProps.NearZ = NearZ;
			m_ViewProps.FarZ = FarZ;

			switch (m_ViewProps.ProjectionType)
			{
			case VT_Perspective:
			{
				const float FOVRadians = FOVDegrees * (3.14f / 180.0f);
				const float AspectRatio = Width / Height;
				m_ViewProps.ProjectionMat = XMMatrixPerspectiveFovLH(FOVRadians, AspectRatio, NearZ, FarZ);
			}
			break;
			case VT_Orthographic:
			{
				m_ViewProps.ProjectionMat = XMMatrixOrthographicLH(Width, Height, NearZ, FarZ);
			}
			break;
			default:
				IE_LOG(Warning, TEXT("Unidentified projection type provided when calculation projection values."))
				break;
			}


		}

	private:

		void UpdateViewMat()
		{
			m_ViewProps.ViewMat = XMMatrixLookAtLH(
				  m_Transform.GetAbsoluteWorldPosition()
				, m_Transform.GetAbsoluteWorldPosition() + m_Transform.GetLocalForward()
				, m_Transform.GetLocalUp()
			);
		}
	};
}
