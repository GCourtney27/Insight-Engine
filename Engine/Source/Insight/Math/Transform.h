#pragma once

#include <Insight/Core.h>

namespace Insight {

	using namespace DirectX; using namespace DirectX::SimpleMath;


	class INSIGHT_API Transform
	{
	public:
		Transform();
		~Transform();
		Transform(const Transform& transform);

		Transform& operator = (const Transform& transform);
		
		inline const Vector3& GetPosition() const { return m_Position; }
		inline const Vector3& GetRotation() const { return m_Rotation; }
		inline const Vector3& GetScale() const { return m_Scale; }

		inline Vector3& GetPositionRef() { m_Transformed = true; return m_Position; }
		inline Vector3& GetRotationRef() { m_Transformed = true; return m_Rotation; }
		inline Vector3& GetScaleRef() { m_Transformed = true; return m_Scale; }

		inline void SetPosition(const Vector3& vector) { m_Position = vector; TranslateLocalMatrix(); UpdateLocalMatrix(); }
		inline void SetRotation(const Vector3& vector) { m_Rotation = vector; RotateLocalMatrix(); UpdateLocalMatrix(); }
		inline void SetScale(const Vector3& vector) { m_Scale = vector; ScaleLocalMatrix(); UpdateLocalMatrix(); }

		inline const Vector3& GetLocalForward() const { return m_LocalForward; }
		inline const Vector3& GetLocalBackward() const { return m_LocalBackward; }
		inline const Vector3& GetLocalLeft() const { return m_LocalLeft; }
		inline const Vector3& GetLocalRight() const { return m_LocalRight; }
		inline const Vector3& GetLocalUp() const { return m_LocalUp; }
		inline const Vector3& GetLocalDown() const { return m_LocalDown; }

		void Translate(float x, float y, float z);
		void Rotate(float x, float y, float z);
		void Scale(float x, float y, float z);

		// Have object look at apoint in space
		void LookAt(const Vector3& lookAtPos);

		// Returns objects local matrix
		XMMATRIX GetLocalMatrix() { UpdateIfTransformed(); return m_LocalMatrix; }
		// Returns a reference to the objects local matrix
		XMMATRIX& GetLocalMatrixRef() { UpdateIfTransformed(); return m_LocalMatrix; }
		// Set the objects local matrix
		void SetLocalMatrix(XMMATRIX matrix);
		XMMATRIX GetLocalMatrixTransposed() const { return XMMatrixTranspose(m_LocalMatrix); }

		// Returns the objects world space matrix
		const XMMATRIX& GetWorldMatrix() { UpdateIfTransformed(); return m_WorldMatrix; }
		// Returns a reference to the objects world space matrix
		XMMATRIX& GetWorldMatrixRef(){ return m_WorldMatrix; }
		// Set the objects world matrix
		void SetWorldMatrix(XMMATRIX matrix);
		XMMATRIX GetWorldMatrixTransposed() const { return XMMatrixTranspose(m_WorldMatrix); }

		// Update the objects local directional vectors.
		// DO NOT CALL UNLESS YOU KNOW WHAT YOU'RE DOING
		void UpdateLocalDirectionVectors();


	protected:

		bool m_Transformed = false;

		void UpdateIfTransformed()
		{
			if (m_Transformed)
			{
				TranslateLocalMatrix();
				ScaleLocalMatrix();
				RotateLocalMatrix();
				UpdateLocalMatrix();
				m_Transformed = false;
			}
		}

		void UpdateLocalMatrix();

		void TranslateLocalMatrix();
		void ScaleLocalMatrix();
		void RotateLocalMatrix();

		XMMATRIX m_LocalMatrix = XMMatrixIdentity();
		XMMATRIX m_WorldMatrix = XMMatrixIdentity();

		XMMATRIX m_TranslationMat = XMMatrixIdentity();
		XMMATRIX m_RotationMat = XMMatrixIdentity();
		XMMATRIX m_ScaleMat = XMMatrixIdentity();

		Vector3 m_Position = m_Position.Zero;
		Vector3 m_Rotation = m_Rotation.Zero;
		Vector3 m_Scale = m_Scale.One;

		Vector3 m_LocalForward = m_LocalForward.Forward;
		Vector3 m_LocalBackward = m_LocalBackward.Backward;
		Vector3 m_LocalLeft = m_LocalLeft.Left;
		Vector3 m_LocalRight = m_LocalRight.Right;
		Vector3 m_LocalUp = m_LocalUp.Up;
		Vector3 m_LocalDown = m_LocalDown.Down;

		const Vector3 WORLD_DIRECTION = WORLD_DIRECTION.Zero;

	};

}
