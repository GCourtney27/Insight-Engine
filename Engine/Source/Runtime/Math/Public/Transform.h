#pragma once

#include <Runtime/Core.h>
#include "Runtime/Math/Public/ie_Matricies.h"

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

		inline FVector GetPosition()	const { return m_Position; }
		inline FVector GetRotation()	const { return m_Rotation; }
		inline FVector GetScale()		const { return m_Scale; }

		inline void SetPosition(float x, float y, float z) { SetPosition(FVector(x, y, z)); TranslateLocalMatrix(); UpdateLocalMatrix(); }
		inline void SetRotation(float XInDegrees, float YInDegrees, float ZInDegrees) { SetRotation(FVector(XInDegrees, YInDegrees, ZInDegrees)); RotateLocalMatrix(); UpdateLocalMatrix(); }
		inline void SetScale(float x, float y, float z) { SetScale(FVector(x, y, z)); ScaleLocalMatrix(); UpdateLocalMatrix(); }

		inline void SetPosition(const FVector& vector) { m_Position = vector; TranslateLocalMatrix(); UpdateLocalMatrix(); }
		inline void SetRotation(const FVector& vector) { m_Rotation = vector; RotateLocalMatrix(); UpdateLocalMatrix(); }
		inline void SetScale(const FVector& vector) { m_Scale = vector; ScaleLocalMatrix(); UpdateLocalMatrix(); }

		inline FVector GetLocalForward()	const { return m_LocalForward; }
		inline FVector GetLocalBackward()	const { return m_LocalBackward; }
		inline FVector GetLocalLeft()		const { return m_LocalLeft; }
		inline FVector GetLocalRight()		const { return m_LocalRight; }
		inline FVector GetLocalUp()			const { return m_LocalUp; }
		inline FVector GetLocalDown()		const { return m_LocalDown; }

		void Translate(float x, float y, float z);
		void Rotate(float XInDegrees, float YInDegrees, float ZInDegrees);
		void Scale(float x, float y, float z);

		// Have object look at a point in space
		void LookAt(FVector& LookAtPos);
		void SetDirection(FVector& NewDirection);

		// Returns objects local matrix
		ieMatrix GetLocalMatrix() { UpdateIfTransformed(); return m_LocalMatrix; }
		// Returns a reference to the objects local matrix
		ieMatrix& GetLocalMatrixRef() { UpdateIfTransformed(); return m_LocalMatrix; }
		// Set the objects local matrix
		void SetLocalMatrix(ieMatrix matrix);
		ieMatrix GetLocalMatrixTransposed() const { return XMMatrixTranspose(m_LocalMatrix); }

		// Returns the objects world space matrix
		ieMatrix GetWorldMatrix() { UpdateIfTransformed(); return m_WorldMatrix; }
		// Returns a reference to the objects world space matrix
		ieMatrix& GetWorldMatrixRef() { return m_WorldMatrix; }
		// Set the objects world matrix
		void SetWorldMatrix(ieMatrix matrix);
		ieMatrix GetWorldMatrixTransposed() const { return XMMatrixTranspose(m_WorldMatrix); }

		ieMatrix& GetTranslationMatrixRef() { return m_TranslationMat; }
		ieMatrix GetTranslationMatrix() { return m_TranslationMat; }
		void SetTranslationMatrix(ieMatrix matrix) { m_TranslationMat = matrix; UpdateLocalMatrix(); }

		ieMatrix& GetRotationMatrixRef() { return m_RotationMat; }
		ieMatrix GetRotationMatrix() { return m_RotationMat; }
		void SetRotationMatrix(ieMatrix matrix) { m_RotationMat = matrix; UpdateLocalMatrix(); }

		ieMatrix& GetScaleMatrixRef() { return m_ScaleMat; }
		ieMatrix GetScaleMatrix() { return m_ScaleMat; }
		void SetScaleMatrix(ieMatrix matrix) { m_ScaleMat = matrix; UpdateLocalMatrix(); }


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

		ieMatrix m_LocalMatrix = DirectX::XMMatrixIdentity();
		ieMatrix m_WorldMatrix = DirectX::XMMatrixIdentity();

		ieMatrix m_TranslationMat = DirectX::XMMatrixIdentity();
		ieMatrix m_RotationMat = DirectX::XMMatrixIdentity();
		ieMatrix m_ScaleMat = DirectX::XMMatrixIdentity();

		FVector m_Position;
		FVector m_Rotation;
		FVector m_Scale;

		FVector m_EditorPlayOriginPosition = m_Position;
		FVector m_EditorPlayOriginRotation = m_Rotation;
		FVector m_EditorPlayOriginScale = m_Scale;


		FVector m_LocalForward = FVector::Forward;
		FVector m_LocalBackward = FVector::Backward;
		FVector m_LocalLeft = FVector::Left;
		FVector m_LocalRight = FVector::Right;
		FVector m_LocalUp = FVector::Up;
		FVector m_LocalDown = FVector::Down;

	};

}
