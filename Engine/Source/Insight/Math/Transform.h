#include "Insight/Core.h"

#include "Insight/Math/Vector.h"

#include <DirectXMath.h>

namespace Insight {

	using namespace DirectX;

	class Transform
	{
	public:
		Transform() {}
		Transform(const Transform& transform);

		void Update() { UpdateMatrix(); }

		Transform& operator = (const Transform& transform)
		{
			m_PositionVector = transform.m_PositionVector;
			m_RotationVector = transform.m_RotationVector;
			m_ScaleVector = transform.m_ScaleVector;

			m_Position = transform.m_Position;
			m_Rotation = transform.m_Rotation;
			m_Scale = transform.m_Scale;

			vec_forward = transform.vec_forward;
			vec_left = transform.vec_left;
			vec_right = transform.vec_right;
			vec_backward = transform.vec_backward;

			vec_forward_noY = transform.vec_forward_noY;
			vec_left_noY = transform.vec_left_noY;
			vec_right_noY = transform.vec_right_noY;
			vec_backward_noY = transform.vec_backward_noY;

			return *this;
		}

		// -- Return and grant modification -- //
		// Returns Position of object in local space
		inline XMFLOAT3& GetPosition() { return m_Position; }
		inline XMFLOAT3 GetPositionConst() const { return m_Position; }
		inline XMFLOAT3 GetPositionCopy() { return m_Position; }
		// Returns Rotation of object in localspace
		inline XMFLOAT3& GetRotation() { return m_Rotation; }
		inline XMFLOAT3 GetRotationConst() const { return m_Rotation; }
		// Returns Scale of object in local space
		inline XMFLOAT3& GetScale() { return m_Scale; }
		inline XMFLOAT3 GetScaleConst() const { return m_Scale; }


		//Returns Position Vector
		inline XMVECTOR& GetPositionVectorRef() { return m_PositionVector; }
		//Returns Rotation Vector
		inline XMVECTOR& GetRotationVectorRef() { return m_RotationVector; }
		//Returns Scale Vector
		inline XMVECTOR& GetRotationScaleRef() { return m_ScaleVector; }

		// Return but do not grant modification
		// (Const) Returns Position Vector of object in local Space
		inline const XMVECTOR& GetPositionVector() const { return m_PositionVector; }
		// (Const) Returns Position Float3 of object in local Space
		inline const XMFLOAT3& GetPositionFloat3() const { return m_Position; }
		// (Const) Returns Rotation Vector of object in local Space
		inline const XMVECTOR& GetRotationVector() const { return m_RotationVector; }
		// (Const) Returns Rotation Float3 of object in local Space
		inline const XMFLOAT3& GetRotationFloat3() const { return m_Rotation; }
		// (Const) Returns Scale Vector of object in local Space
		inline const XMVECTOR& GetScaleVector() const { return m_ScaleVector; }
		// (Const) Returns Scale Float3 of object in local Space
		inline const XMFLOAT3& GetScaleFloat3() const { return m_Scale; }

		// All operations end up storing as a XMFLOAT3 to updata the local matrix



		/* Position */
		// Immediately set position with Vector, no blending can be done
		void SetPosition(const XMVECTOR& pos);
		// Immediately set position with Float3, no blending can be done
		void SetPosition(const XMFLOAT3& pos);
		// Immediately set position with float x y z, no blending can be done
		void SetPosition(float x, float y, float z);
		// Adds value to position with Vector, can be blended. Good for moving with delta time for example
		void AdjustPosition(const XMVECTOR& pos);
		// Adds value to position with Float3, can be blended. Good for moving with delta time for example
		void AdjustPosition(const XMFLOAT3& pos);
		// Adds value to position with float x y z, can be blended. Good for moving with delta time for example
		void AdjustPosition(float x, float y, float z);

		/* Rotation */
		// Immediately set rotation with Vector, no blending can be done
		void SetRotation(const XMVECTOR& rot);
		// Immediately set rotation with Float3, no blending can be done
		void SetRotation(const XMFLOAT3& rot);
		// Immediately set rotation with float x y z, no blending can be done
		void SetRotation(float x, float y, float z);
		// Adds value to rotation with Vector, can be blended. Good for moving with delta time for example
		void AdjustRotation(const XMVECTOR& rot);
		// Adds value to rotation with Float3, can be blended. Good for moving with delta time for example
		void AdjustRotation(const XMFLOAT3& rot);
		// Adds value to rotation with float x y z, can be blended. Good for moving with delta time for example
		void AdjustRotation(float x, float y, float z);

		/* Scale */
		// Immediately set scale with Vector, no blending can be done
		void SetScale(const XMVECTOR& scale);
		// Immediately set scale with Float3, no blending can be done
		void SetScale(const XMFLOAT3& scale);
		// Immediately set scale with float x y z, no blending can be done
		void SetScale(float xScale, float yScale, float zScale = 1.0f);
		// Adds value to scale with Vector, can be blended. Good for moving with delta time for example
		void AdjustScale(const XMVECTOR& scale);
		// Adds value to scale with Float3, can be blended. Good for moving with delta time for example
		void AdjustScale(const XMFLOAT3& scale);
		// Adds value to scale with float x y z, can be blended. Good for moving with delta time for example
		void AdjustScale(float x, float y, float z);

		/* This objects default directional vectors*/
		// Returns objects local forward vector
		const XMVECTOR& GetForwardVector(bool omitY = false);
		// Returns objects local backward vector
		const XMVECTOR& GetBackwardVector(bool omitY = false);
		// Returns objects local left vector
		const XMVECTOR& GetLeftVector(bool omitY = false);
		// Returns objects local right vector
		const XMVECTOR& GetRightVector(bool omitY = false);
		// Returns objects local up vector
		const XMVECTOR& GetUpVector();
		// Returns objects local down vector
		const XMVECTOR& GetDownVector();

		// Have object look at an object
		void SetLookAtPos(XMFLOAT3 lookAtPos);

		// Returns objects local matrix
		XMMATRIX GetLocalMatrix() const { return m_LocalMatrix; }
		// Set the objects local matrix
		void SetLocalMatrix(XMMATRIX matrix);

		// Returns the objets world matrix
		XMMATRIX GetWorldMatrix() const
		{
			return m_WorldMatrix;
		}
		// Set the objects world matrix
		void SetWorldMatrix(XMMATRIX matrix);

		// DO NOT CALL UNLESS YOU KNOW WHAT YOU'RE DOING
		void UpdateDirectionVectors();

		void SetParent(Transform* parent) { m_pParent = parent; }

		const XMVECTOR GetDefaultUpVector() const { return DEFAULT_UP_VECTOR; }
		const XMVECTOR GetDefaultDownVector() const { return DEFAULT_DOWN_VECTOR; }
		const XMVECTOR GetDefaultLeftVector() const { return DEFAULT_LEFT_VECTOR; }
		const XMVECTOR GetDefaultRightVector() const { return DEFAULT_RIGHT_VECTOR; }
		const XMVECTOR GetDefaultForwardVector() const { return DEFAULT_FOREWARD_VECTOR; }
		const XMVECTOR GetDefaultBackwardVector() const { return DEFAULT_BACKWARD_VECTOR; }

	protected:

		void UpdateMatrix();

		XMMATRIX m_LocalMatrix = XMMatrixIdentity();
		XMMATRIX m_WorldMatrix = XMMatrixIdentity();

		Transform* m_pParent = nullptr;

		// XMVECTOR
		XMVECTOR m_PositionVector;
		XMVECTOR m_RotationVector;
		XMVECTOR m_ScaleVector;

		// XMFLOAT3
		XMFLOAT3 m_Position;
		XMFLOAT3 m_Rotation;
		XMFLOAT3 m_Scale;

		const XMVECTOR DEFAULT_FOREWARD_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
		const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
		const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		const XMVECTOR DEFAULT_DOWN_VECTOR = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);

		XMVECTOR vec_forward;
		XMVECTOR vec_left;
		XMVECTOR vec_right;
		XMVECTOR vec_backward;

		XMVECTOR vec_forward_noY;
		XMVECTOR vec_left_noY;
		XMVECTOR vec_right_noY;
		XMVECTOR vec_backward_noY;
	};

}
