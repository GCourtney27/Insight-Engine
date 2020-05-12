#pragma once

#include <Insight/Core.h>

namespace Insight {

	using namespace DirectX; using namespace DirectX::SimpleMath;


	class INSIGHT_API Transform
	{
	public:
		Transform();
		Transform(const Transform& transform);

		Transform& operator = (const Transform& transform)
		{
			m_Position = transform.m_Position;
			m_Rotation = transform.m_Rotation;
			m_Scale = transform.m_Scale;

			m_Position = transform.m_Position;
			m_Rotation = transform.m_Rotation;
			m_Scale = transform.m_Scale;

			return *this;
		}

		inline const Vector3& GetPosition() const { return m_Position; }
		inline const Vector3& GetRotation() const { return m_Rotation; }
		inline const Vector3& GetScale() const { return m_Scale; }

		inline Vector3& GetPositionRef() { return m_Position; }
		inline Vector3& GetRotationRef() { return m_Rotation; }
		inline Vector3& GetScaleRef() { return m_Scale; }

		inline void SetPosition(const Vector3& vector) { m_Position = vector; UpdateMatrix(); }
		inline void SetRotation(const Vector3& vector) { m_Rotation = vector; UpdateMatrix(); }
		inline void SetScale(const Vector3& vector) { m_Scale = vector; UpdateMatrix(); }

		inline const Vector3& GetLocalForward() const { return m_LocalForward; }
		inline const Vector3& GetLocalBackward() const { return m_LocalBackward; }
		inline const Vector3& GetLocalLeft() const { return m_LocalLeft; }
		inline const Vector3& GetLocalRight() const { return m_LocalRight; }
		inline const Vector3& GetLocalUp() const { return m_LocalUp; }
		inline const Vector3& GetLocalDown() const { return m_LocalDown; }

		void Translate(float x, float y, float z);
		void Rotate(float x, float y, float z);
		void Scale(float x, float y, float z);

		// Have object look at an object
		inline void LookAt(const Vector3& lookAtPos);

		// Returns objects local matrix
		XMMATRIX GetLocalMatrix() const { return m_LocalMatrix; }
		XMMATRIX& GetLocalMatrixRef() { return m_LocalMatrix; }
		// Set the objects local matrix
		void SetLocalMatrix(XMMATRIX matrix);
		XMMATRIX GetLocalMatrixTransposed() const { return XMMatrixTranspose(m_LocalMatrix); }

		// Returns the objets world matrix
		XMMATRIX GetWorldMatrix() const	{ return m_WorldMatrix; }
		XMMATRIX& GetWorldMatrixRef(){ return m_WorldMatrix; }
		// Set the objects world matrix
		void SetWorldMatrix(XMMATRIX matrix);
		XMMATRIX GetWorldMatrixTransposed() const { return XMMatrixTranspose(m_WorldMatrix); }

		// DO NOT CALL UNLESS YOU KNOW WHAT YOU'RE DOING
		void UpdateLocalDirectionVectors();


	protected:

		void UpdateMatrix();

		void TranslateLocalMatrix();
		void ScaleLocalMatrix();
		void RotateLocalMatrix();

		XMMATRIX m_LocalMatrix = XMMatrixIdentity();
		XMMATRIX m_WorldMatrix = XMMatrixIdentity();

		XMMATRIX m_TranslationMat;
		XMMATRIX m_RotationMat;
		XMMATRIX m_ScaleMat;

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
