#pragma once

#include <Insight/Core.h>
#include "Insight/Math/ie_Vectors.h"
#include "Insight/Math/ie_Matricies.h"

namespace Insight {

	using namespace DirectX;
	using namespace Math;

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

		inline const ieVector3& GetPosition()		const { return m_Position; }
		inline const ieVector3& GetRotation()		const { return m_Rotation; }
		inline const ieVector3& GetScale()		const { return m_Scale; }

		inline ieVector3& GetPositionRef()	{ m_Transformed = true; return m_Position; }
		inline ieVector3& GetRotationRef()	{ m_Transformed = true; return m_Rotation; }
		inline ieVector3& GetScaleRef()		{ m_Transformed = true; return m_Scale; }

		inline void SetPosition(float x, float y, float z)	{ m_Position.x = x; m_Position.y = y; m_Position.z = z; TranslateLocalMatrix(); UpdateLocalMatrix(); }
		inline void SetRotation(float XInDegrees, float YInDegrees, float ZInDegrees)	{ m_Rotation.x = (XInDegrees); m_Rotation.y = (YInDegrees); m_Rotation.z = (ZInDegrees); RotateLocalMatrix(); UpdateLocalMatrix(); }
		inline void SetScale(float x, float y, float z)		{ m_Scale.x = x; m_Scale.y = y; m_Scale.z = z; ScaleLocalMatrix(); UpdateLocalMatrix(); }

		inline void SetPosition(const ieVector3& vector)	{ m_Position = vector; TranslateLocalMatrix(); UpdateLocalMatrix(); }
		inline void SetRotation(const ieVector3& vector)	{ m_Rotation = vector; RotateLocalMatrix(); UpdateLocalMatrix(); }
		inline void SetScale(const ieVector3& vector)		{ m_Scale = vector; ScaleLocalMatrix(); UpdateLocalMatrix(); }

		inline const ieVector3& GetLocalForward()		const { return m_LocalForward; }
		inline const ieVector3& GetLocalBackward()	const { return m_LocalBackward; }
		inline const ieVector3& GetLocalLeft()		const { return m_LocalLeft; }
		inline const ieVector3& GetLocalRight()		const { return m_LocalRight; }
		inline const ieVector3& GetLocalUp()			const { return m_LocalUp; }
		inline const ieVector3& GetLocalDown()		const { return m_LocalDown; }

		void Translate(float x, float y, float z);
		void Rotate(float XInDegrees, float YInDegrees, float ZInDegrees);
		void Scale(float x, float y, float z);

		// Have object look at a point in space
		void LookAt(const ieVector3& LookAtPos);
		void SetDirection(const ieVector3& NewDirection);

		// Returns objects local matrix
		ieMatrix GetLocalMatrix() { UpdateIfTransformed(); return m_LocalMatrix; }
		// Returns a reference to the objects local matrix
		ieMatrix& GetLocalMatrixRef() { UpdateIfTransformed(); return m_LocalMatrix; }
		// Set the objects local matrix
		void SetLocalMatrix(ieMatrix matrix);
		ieMatrix GetLocalMatrixTransposed() const { return XMMatrixTranspose(m_LocalMatrix); }

		// Returns the objects world space matrix
		const ieMatrix& GetWorldMatrix() { UpdateIfTransformed(); return m_WorldMatrix; }
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

		XMMATRIX m_LocalMatrix = XMMatrixIdentity();
		XMMATRIX m_WorldMatrix = XMMatrixIdentity();

		XMMATRIX m_TranslationMat = XMMatrixIdentity();
		XMMATRIX m_RotationMat = XMMatrixIdentity();
		XMMATRIX m_ScaleMat = XMMatrixIdentity();

		ieVector3 m_Position = m_Position.Zero;
		ieVector3 m_Rotation = m_Rotation.Zero;
		ieVector3 m_Scale = m_Scale.One;
		
		ieVector3 m_EditorPlayOriginPosition = m_Position;
		ieVector3 m_EditorPlayOriginRotation = m_Rotation;
		ieVector3 m_EditorPlayOriginScale = m_Scale;
		
		ieVector3 m_LocalForward = m_LocalForward.Forward;
		ieVector3 m_LocalBackward = m_LocalBackward.Backward;
		ieVector3 m_LocalLeft = m_LocalLeft.Left;
		ieVector3 m_LocalRight = m_LocalRight.Right;
		ieVector3 m_LocalUp = m_LocalUp.Up;
		ieVector3 m_LocalDown = m_LocalDown.Down;

	};

}
