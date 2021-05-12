#pragma once

#include <Runtime/Core.h>
#include "Runtime/Core/Public/ieObject/ieActor.h"

#include "Runtime/Math/Public/Transform.h"

namespace Insight
{

	class INSIGHT_API ieCameraActor : public ieActor
	{
		struct CameraViewProperties
		{
			float NearZ;
			float FarZ;
			FMatrix ViewMat;
			FMatrix ProjMat;
		};
	public:
		ieCameraActor(ieWorld* pWorld, FVector2 ViewPortDims)
			: ieActor(pWorld, L"ieCameraActor")
		{
			UpdateViewMat();
			SetProjectionValues(45.f, ViewPortDims.x / ViewPortDims.y, 0.1f, 1000.f);
		}
		virtual ~ieCameraActor() {}

		virtual void Tick(float DeltaMs) override
		{
			ieActor::Tick(DeltaMs);

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
			return m_ViewProps.ProjMat;
		}

		inline float GetNearZ()
		{
			return m_ViewProps.NearZ;
		}

		inline float GetFarZ()
		{
			return m_ViewProps.FarZ;
		}

		void SetProjectionValues(float FOVDegrees, float AspectRatio, float NearZ, float FarZ)
		{
			m_ViewProps.NearZ = NearZ;
			m_ViewProps.FarZ = FarZ;
			float fovRadians = FOVDegrees * (3.14f / 180.0f);
			m_ViewProps.ProjMat = XMMatrixPerspectiveFovLH(fovRadians, AspectRatio, NearZ, FarZ);
		}

	private:

		void UpdateViewMat()
		{
			FVector3 Target = m_Transform.GetPosition() + m_Transform.GetLocalForward();
			m_ViewProps.ViewMat = XMMatrixLookAtLH(m_Transform.GetPosition(), Target, m_Transform.GetLocalUp());
		}

		ieTransform m_Transform;
		CameraViewProperties m_ViewProps;
	};
}
