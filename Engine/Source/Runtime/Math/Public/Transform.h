#pragma once

#include <Runtime/Core.h>

#include "Runtime/Math/Public/Vectors.h"
#include "Runtime/Math/Public/Matricies.h"
#include "Runtime/Math/Public/Quaternion.h"

namespace Insight
{
	
	class INSIGHT_API ieTransform
	{
	public:
		ieTransform();
		FORCE_INLINE ~ieTransform() = default;
		FORCE_INLINE ieTransform(const ieTransform& Transform)
		{
			*this = Transform;
		}
		FORCE_INLINE ieTransform& operator = (const ieTransform& Transform)
		{
			SetPosition(Transform.m_Position);
			SetRotation(Transform.m_Rotation);
			SetScale(Transform.m_Scale);
			return *this;
		}

		void EditorEndPlay();
		void EditorInit() { UpdateEditorOriginPositionRotationScale(); }

		inline FVector3 GetPosition()	const { return m_Position; }
		inline FVector3 GetRotation()	const { return m_Rotation; }
		inline FVector3 GetScale()		const { return m_Scale; }

		inline void SetPosition(float X, float Y, float Z)
		{
			SetPosition(FVector3(X, Y, Z));
		}
		/*
			Set the rotation of the transform using Pitch, Yaw, Roll in degrees.
		*/
		inline void SetRotation(float Pitch, float Yaw, float Roll)
		{
			SetRotation(FVector3(Pitch, Yaw, Roll));
		}
		inline void SetScale(float X, float Y, float Z)
		{
			SetScale(FVector3(X, Y, Z));
		}
		inline void SetPosition(const FVector3& Position) 
		{ 
			m_Position = Position;
			TranslateLocalMatrix(); 
			UpdateLocalMatrix(); 
		}
		inline void SetRotation(const FVector3& Rotation)
		{
			m_Rotation = Rotation;
			RotateLocalMatrix();
			UpdateLocalMatrix();
		}
		inline void SetScale(const FVector3& Scale) 
		{ 
			m_Scale = Scale;
			ScaleLocalMatrix(); 
			UpdateLocalMatrix(); 
		}

		inline FVector3 GetLocalUp()		const { return m_LocalUp;		}
		inline FVector3 GetLocalDown()		const { return m_LocalDown;		}
		inline FVector3 GetLocalLeft()		const { return m_LocalLeft;		}
		inline FVector3 GetLocalRight()		const { return m_LocalRight;	}
		inline FVector3 GetLocalForward()	const { return m_LocalForward;	}
		inline FVector3 GetLocalBackward()	const { return m_LocalBackward; }

		inline void Translate(float X, float Y, float Z)
		{
			m_Position.x += X;
			m_Position.y += Y;
			m_Position.z += Z;
			TranslateLocalMatrix();
			UpdateLocalMatrix();
		}
		inline void Rotate(float Pitch, float Yaw, float Roll)
		{
			m_Rotation.x += Pitch;
			m_Rotation.y += Yaw;
			m_Rotation.z += Roll;
			RotateLocalMatrix();
			UpdateLocalMatrix();
		}
		inline void Scale(float X, float Y, float Z)
		{
			m_Scale.x += X;
			m_Scale.y += Y;
			m_Scale.z += Z;
			ScaleLocalMatrix();
			UpdateLocalMatrix();
		}

		void LookAt(FVector3& LookAtPos);

		// Returns objects local matrix
		FMatrix GetLocalMatrix() { UpdateIfTransformed(); return m_LocalMatrix; }
		// Returns a reference to the objects local matrix
		FMatrix& GetLocalMatrixRef() { UpdateIfTransformed(); return m_LocalMatrix; }
		// Set the objects local matrix
		void SetLocalMatrix(const FMatrix& matrix);
		FMatrix GetLocalMatrixTransposed() const { return XMMatrixTranspose(m_LocalMatrix); }

		// Returns the objects world space matrix
		FMatrix GetWorldMatrix() { UpdateIfTransformed(); return m_WorldMatrix; }
		// Returns a reference to the objects world space matrix
		FMatrix& GetWorldMatrixRef() { return m_WorldMatrix; }
		// Set the objects world matrix
		void SetWorldMatrix(const FMatrix& matrix);
		FMatrix GetWorldMatrixTransposed() const { return XMMatrixTranspose(m_WorldMatrix); }

		FMatrix& GetTranslationMatrixRef() { return m_TranslationMat; }
		FMatrix GetTranslationMatrix() { return m_TranslationMat; }
		void SetTranslationMatrix(FMatrix matrix) { m_TranslationMat = matrix; UpdateLocalMatrix(); }

		FMatrix& GetRotationMatrixRef() { return m_RotationMat; }
		FMatrix GetRotationMatrix() { return m_RotationMat; }
		void SetRotationMatrix(FMatrix matrix) { m_RotationMat = matrix; UpdateLocalMatrix(); }

		FMatrix& GetScaleMatrixRef() { return m_ScaleMat; }
		FMatrix GetScaleMatrix() { return m_ScaleMat; }
		void SetScaleMatrix(FMatrix matrix) { m_ScaleMat = matrix; UpdateLocalMatrix(); }


		void UpdateEditorOriginPositionRotationScale();
	protected:

		bool m_Transformed = false;
		void UpdateIfTransformed(bool ForceUpdate = false);

		inline void RotateVector(FVector3& outResult, const FVector3& Direction, const FMatrix& Matrix)
		{
			outResult = XMVector3TransformCoord(Direction, m_RotationMat);
		}

		// Matrix Operations
		//
		void UpdateLocalMatrix();
		void TranslateLocalMatrix();
		void ScaleLocalMatrix();
		void RotateLocalMatrix();

		FMatrix m_LocalMatrix = DirectX::XMMatrixIdentity();
		FMatrix m_WorldMatrix = DirectX::XMMatrixIdentity();

		FMatrix m_TranslationMat = DirectX::XMMatrixIdentity();
		FMatrix m_RotationMat = DirectX::XMMatrixIdentity();
		FMatrix m_ScaleMat = DirectX::XMMatrixIdentity();

		FVector3 m_Position;
		FVector3 m_Rotation;
		FVector3 m_Scale;

		FVector3 m_EditorPlayOriginPosition;
		FVector3 m_EditorPlayOriginRotation;
		FVector3 m_EditorPlayOriginScale;

		FVector3 m_LocalForward = FVector3::Forward;
		FVector3 m_LocalBackward = FVector3::Backward;
		FVector3 m_LocalLeft = FVector3::Left;
		FVector3 m_LocalRight = FVector3::Right;
		FVector3 m_LocalUp = FVector3::Up;
		FVector3 m_LocalDown = FVector3::Down;

	};

}
