#pragma once

#include <Runtime/Core.h>
#include "Runtime/Core/Public/ieObject/Components/ieComponentBase.h"

#include "Runtime/Math/Public/Transform.h"



namespace Insight
{
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

	class INSIGHT_API ieCameraComponent : public ieComponentBase<ieCameraComponent>
	{
	protected:
		friend class CameraSystem;

		ieTransform m_Transform;
		ProjectionProperties m_ViewProps;

	public:
		ieCameraComponent()
		{
			UpdateViewMat();
			
			SetProjectionValues(45.f, 1600.f, 900.f, 0.001f, 1000.f);
		}
		virtual ~ieCameraComponent() {}
		ieCameraComponent& operator=(const ieCameraComponent& Other) = default;

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
				  m_Transform.GetPosition()
				, m_Transform.GetPosition() + m_Transform.GetLocalForward()
				, m_Transform.GetLocalUp()
			);
		}
	};
}
