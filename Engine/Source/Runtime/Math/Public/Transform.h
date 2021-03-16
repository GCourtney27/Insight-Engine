#pragma once

#include <Runtime/Core.h>

#include "Runtime/Math/Public/Vectors.h"
#include "Runtime/Math/Public/Matricies.h"

namespace Insight 
{

	class INSIGHT_API ieTransform
	{
	public:
		ieTransform();
		~ieTransform();
		ieTransform(const ieTransform& transform);
		ieTransform(ieTransform&& transform) noexcept;

		ieTransform& operator = (const ieTransform& transform);

		void EditorEndPlay();
		void EditorInit() { UpdateEditorOriginPositionRotationScale(); }

		inline FVector3 GetPosition()	const { return m_Position; }
		inline FVector3 GetRotation()	const { return m_Rotation; }
		inline FVector3 GetScale()		const { return m_Scale; }

		inline void SetPosition(float x, float y, float z) 
		{ SetPosition(FVector3(x, y, z)); TranslateLocalMatrix(); UpdateLocalMatrix(); }
		
		inline void SetRotation(float XInDegrees, float YInDegrees, float ZInDegrees)
		{ SetRotation(FVector3(XInDegrees, YInDegrees, ZInDegrees)); RotateLocalMatrix(); UpdateLocalMatrix(); }
		
		inline void SetScale(float x, float y, float z) 
		{ SetScale(FVector3(x, y, z)); ScaleLocalMatrix(); UpdateLocalMatrix(); }

		inline void SetPosition(const FVector3& vector) { m_Position = vector; TranslateLocalMatrix(); UpdateLocalMatrix(); }
		inline void SetRotation(const FVector3& vector) { m_Rotation = vector; RotateLocalMatrix(); UpdateLocalMatrix(); }
		inline void SetScale(const FVector3& vector) { m_Scale = vector; ScaleLocalMatrix(); UpdateLocalMatrix(); }

		inline FVector3 GetLocalForward()	const { return m_LocalForward; }
		inline FVector3 GetLocalBackward()	const { return m_LocalBackward; }
		inline FVector3 GetLocalLeft()		const { return m_LocalLeft; }
		inline FVector3 GetLocalRight()		const { return m_LocalRight; }
		inline FVector3 GetLocalUp()			const { return m_LocalUp; }
		inline FVector3 GetLocalDown()		const { return m_LocalDown; }

		void Translate(float x, float y, float z);
		void Rotate(float XInDegrees, float YInDegrees, float ZInDegrees);
		void Scale(float x, float y, float z);

		// Have object look at a point in space
		void LookAt(FVector3& LookAtPos);
		void SetDirection(FVector3& NewDirection);

		// Returns objects local matrix
		FMatrix GetLocalMatrix() { UpdateIfTransformed(); return m_LocalMatrix; }
		// Returns a reference to the objects local matrix
		FMatrix& GetLocalMatrixRef() { UpdateIfTransformed(); return m_LocalMatrix; }
		// Set the objects local matrix
		void SetLocalMatrix(FMatrix matrix);
		FMatrix GetLocalMatrixTransposed() const { return XMMatrixTranspose(m_LocalMatrix); }

		// Returns the objects world space matrix
		FMatrix GetWorldMatrix() { UpdateIfTransformed(); return m_WorldMatrix; }
		// Returns a reference to the objects world space matrix
		FMatrix& GetWorldMatrixRef() { return m_WorldMatrix; }
		// Set the objects world matrix
		void SetWorldMatrix(FMatrix matrix);
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


		// Update the objects local directional vectors.
		// DO NOT CALL UNLESS YOU KNOW WHAT YOU'RE DOING
		void UpdateLocalDirectionVectors();

		void UpdateEditorOriginPositionRotationScale();
	protected:

		bool m_Transformed = false;

		void UpdateIfTransformed(bool ForceUpdate = false);


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

		FVector3 m_EditorPlayOriginPosition = m_Position;
		FVector3 m_EditorPlayOriginRotation = m_Rotation;
		FVector3 m_EditorPlayOriginScale = m_Scale;


		FVector3 m_LocalForward = FVector3::Forward;
		FVector3 m_LocalBackward = FVector3::Backward;
		FVector3 m_LocalLeft = FVector3::Left;
		FVector3 m_LocalRight = FVector3::Right;
		FVector3 m_LocalUp = FVector3::Up;
		FVector3 m_LocalDown = FVector3::Down;

	};

}
