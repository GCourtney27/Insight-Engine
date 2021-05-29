// Copyright Insight Interactive. All Rights Reserved.
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
		ieTransform::ieTransform()
			: m_pParent(NULL)
			, m_Position(FVector3::Zero)
			, m_Rotation(FVector3::Zero)
			, m_Scale(FVector3::One)
		{
		}
		ieTransform::~ieTransform()
		{
			ReAssignChildrenToParent();
			UnsetParent();
		}
		ieTransform(ieTransform&& Other) noexcept
		{
			m_Scale		= Other.m_Scale;
			m_Position	= Other.m_Position;
			m_Rotation	= Other.m_Rotation;
			m_pParent	= Other.m_pParent;
			m_Children	= std::move(Other.m_Children);

			ComputeAllMatriciesAndUpdateChildren();

			Other.m_pParent = NULL;
		}
		ieTransform(const ieTransform& Transform)
		{
			*this = Transform;
		}
		ieTransform& operator = (const ieTransform& Other)
		{
			m_Position	= Other.m_Position;
			m_Scale		= Other.m_Scale;
			m_Rotation	= Other.m_Rotation;

			ComputeAllMatriciesAndUpdateChildren();
			return *this;
		}


		/*
			Returns the parent of this transform. Null if no parent.
		*/
		inline ieTransform* GetParent() const { return m_pParent; }
		inline void SetParent(ieTransform* NewParent) 
		{
			m_pParent = NewParent; 
			if (m_pParent != NULL)
			{
				m_pParent->AddChild(*this);
			}
			ComputeWorldMatrix();
		}
		inline void UnsetParent() 
		{ 
			if (m_pParent != NULL)
			{
				m_pParent->RemoveChild(this);
				m_pParent = NULL; 
			}
		}

		/*
			Returns the objects position relative to it's parent.
		*/
		inline FVector3 GetPosition()	const { return m_Position; }
		inline FVector3 GetRotation()	const { return m_Rotation; }
		inline FVector3 GetScale()		const { return m_Scale; }

		/*
			Returns the absolute world position of this actor. As in, 
			it's position in world space not relative to it's parent (if it has one).
		*/
		inline FVector3 GetAbsoluteWorldPosition()
		{
			if (m_pParent != NULL)
			{
				return m_pParent->GetPosition() + m_Position;
			}
			return m_Position;
		}

		/*
			Set the position of the object relative to its parent.
		*/
		inline void SetPosition(float X, float Y, float Z);
		
		/*
			Set the rotation of the transform using Pitch, Yaw, Roll in degrees relative to its parent.
		*/
		inline void SetRotation(float Pitch, float Yaw, float Roll);
		
		/*
			Set the scale of the object relative to its parent.
		*/
		inline void SetScale(float X, float Y, float Z);

		inline void SetPosition(const FVector3& Position);
		inline void SetRotation(const FVector3& Rotation);
		inline void SetScale(const FVector3& Scale);

		/*
			Returns the transform local UP vector.
		*/
		inline FVector3 GetLocalUp();

		/*
			Returns the transform local DOWN vector.
		*/
		inline FVector3 GetLocalDown();
		
		/*
			Returns the transform local LEFT vector.
		*/
		inline FVector3 GetLocalLeft();
		
		/*
			Returns the transform local RIGHT vector.
		*/
		inline FVector3 GetLocalRight();
		
		/*
			Returns the transform local FORWARD vector.
		*/
		inline FVector3 GetLocalForward();
		
		/*
			Returns the transform local BACKWARD vector.
		*/
		inline FVector3 GetLocalBackward();


		inline void Translate(const FVector3& Translation);
		inline void Rotate(const FVector3& Rotation);
		inline void Scale(const FVector3& NewScale);

		inline void Translate(float X, float Y, float Z);
		inline void Rotate(float Pitch, float Yaw, float Roll);
		inline void Scale(float X, float Y, float Z);

		/*
			Transform the point to look at a point in space.
		*/
		void LookAt(const FVector3& LookAtPos);

		/*
			Returns objects local matrix.
		*/
		FMatrix GetLocalMatrix() { return m_LocalMatrix; }
		/*
			Returns a reference to the objects local matrix
		*/
		FMatrix& GetLocalMatrixRef() { return m_LocalMatrix; }
		/*
			Set the objects local matrix
		*/
		void SetLocalMatrix(const FMatrix& matrix);
		FMatrix GetLocalMatrixTransposed() const { return XMMatrixTranspose(m_LocalMatrix); }

		/*
			Returns the objects world space matrix
		*/
		FMatrix GetWorldMatrix() { return m_WorldMatrix; }
		/*
			Returns a reference to the objects world space matrix
		*/
		FMatrix& GetWorldMatrixRef() { return m_WorldMatrix; }
		/*
			Set the objects world matrix
		*/
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


	protected:

		inline void RotateVector(FVector3& outResult, const FVector3& Direction, const FMatrix& Matrix);
		
		/*
			Add a child that will be updated relative to this transform.
		*/
		inline void AddChild(ieTransform& Child);
		/*
			Remove a child from being updated form this transform.
		*/
		inline bool RemoveChild(ieTransform* Child);
		/*
			Repoint the children attached to this tansform to this parent's parent.
		*/
		inline void ReAssignChildrenToParent();
		/*
			Compute the world space matrix of this transform.
		*/
		inline void ComputeWorldMatrix();
		/*
			Update the children relative to this transform.
		*/
		inline void UpdateChildren();


		// Matrix Operations
		//
		void UpdateLocalMatrix();
		void TranslateLocalMatrix();
		void ScaleLocalMatrix();
		void RotateLocalMatrix();
		inline void ComputeAllMatriciesAndUpdateChildren();
		inline void UpdateLocalVectors();

		ieTransform* m_pParent;
		std::vector<ieTransform*> m_Children;

		FMatrix m_LocalMatrix = DirectX::XMMatrixIdentity();
		FMatrix m_WorldMatrix = DirectX::XMMatrixIdentity();

		FMatrix m_TranslationMat = DirectX::XMMatrixIdentity();
		FMatrix m_RotationMat = DirectX::XMMatrixIdentity();
		FMatrix m_ScaleMat = DirectX::XMMatrixIdentity();

		FVector3 m_Position;
		FVector3 m_Rotation;
		FVector3 m_Scale;

		FVector3 m_LocalForward = FVector3::Forward;
		FVector3 m_LocalBackward = FVector3::Backward;
		FVector3 m_LocalLeft = FVector3::Left;
		FVector3 m_LocalRight = FVector3::Right;
		FVector3 m_LocalUp = FVector3::Up;
		FVector3 m_LocalDown = FVector3::Down;

	};


	// 
	// Inline function definitions
	// 

	inline void ieTransform::RotateVector(FVector3& outResult, const FVector3& inTarget, const FMatrix& inRotationMatrix)
	{
		outResult = XMVector3TransformCoord(inTarget, inRotationMatrix);
	}

	inline void ieTransform::AddChild(ieTransform& Child)
	{ 
		m_Children.push_back(&Child); 
	}

	inline bool ieTransform::RemoveChild(ieTransform* Child)
	{
		auto Iter = std::find(m_Children.begin(), m_Children.end(), Child);
		if (Iter != m_Children.end())
		{
			m_Children.erase(Iter);
			return true;
		}
		return false;
	}

	inline void ieTransform::ReAssignChildrenToParent()
	{
		for (size_t i = 0; i < m_Children.size(); ++i)
		{
			m_Children[i]->SetParent(GetParent());
		}
	}

	inline void ieTransform::ComputeWorldMatrix()
	{
		if (m_pParent != NULL)
		{
			m_WorldMatrix = m_LocalMatrix * m_pParent->GetWorldMatrix();
		}
		else
		{
			m_WorldMatrix = m_LocalMatrix;
		}
		UpdateChildren();
	}

	inline void ieTransform::UpdateChildren()
	{
		for (size_t i = 0; i < m_Children.size(); ++i)
		{
			m_Children[i]->ComputeWorldMatrix();
		}
	}

	inline void ieTransform::ComputeAllMatriciesAndUpdateChildren()
	{
		TranslateLocalMatrix();
		ScaleLocalMatrix();
		RotateLocalMatrix();
		UpdateLocalMatrix();
		ComputeWorldMatrix();
		UpdateChildren();
	}
	
	inline void ieTransform::UpdateLocalVectors()
	{
		RotateVector(m_LocalUp, FVector3::Up, m_RotationMat);
		RotateVector(m_LocalDown, FVector3::Down, m_RotationMat);
		RotateVector(m_LocalLeft, FVector3::Left, m_RotationMat);
		RotateVector(m_LocalRight, FVector3::Right, m_RotationMat);
		RotateVector(m_LocalForward, FVector3::Forward, m_RotationMat);
		RotateVector(m_LocalBackward, FVector3::Backward, m_RotationMat);
	}

	inline void ieTransform::Translate(const FVector3& Translation)
	{
		Translate(Translation.x, Translation.y, Translation.z);
	}
	
	inline void ieTransform::Translate(float X, float Y, float Z)
	{
		float NewX = m_Position.x + X;
		float NewY = m_Position.x + Y;
		float NewZ = m_Position.x + Z;
		SetPosition(NewX, NewY, NewZ);
	}
	
	inline void ieTransform::Rotate(const FVector3& Rotation)
	{
		Rotate(Rotation.x, Rotation.y, Rotation.z);
	}
	
	inline void ieTransform::Rotate(float Pitch, float Yaw, float Roll)
	{
		m_Rotation.x += Pitch;
		m_Rotation.y += Yaw;
		m_Rotation.z += Roll;
		RotateLocalMatrix();
		UpdateLocalMatrix();
	}
	
	inline void ieTransform::Scale(const FVector3& NewScale)
	{
		Scale(NewScale.x, NewScale.y, NewScale.z);
	}
	
	inline void ieTransform::Scale(float X, float Y, float Z)
	{
		m_Scale.x += X;
		m_Scale.y += Y;
		m_Scale.z += Z;
		ScaleLocalMatrix();
		UpdateLocalMatrix();
	}

	inline void ieTransform::SetPosition(float X, float Y, float Z)
	{
		m_Position.x = X;
		m_Position.y = Y;
		m_Position.z = Z;
		TranslateLocalMatrix();
		UpdateLocalMatrix();
	}
	
	inline void ieTransform::SetRotation(float Pitch, float Yaw, float Roll)
	{
		m_Rotation.x = Pitch;
		m_Rotation.y = Yaw;
		m_Rotation.z = Roll;
		RotateLocalMatrix();
		UpdateLocalMatrix();
	}

	inline void ieTransform::SetScale(float X, float Y, float Z)
	{
		m_Scale.x = X;
		m_Scale.y = Y;
		m_Scale.z = Z;
		ScaleLocalMatrix();
		UpdateLocalMatrix();
	}
	
	inline void ieTransform::SetPosition(const FVector3& Position)
	{
		SetPosition(Position.x, Position.y, Position.z);
	}
	
	inline void ieTransform::SetRotation(const FVector3& Rotation)
	{
		SetRotation(Rotation.x, Rotation.y, Rotation.z);
	}
	
	inline void ieTransform::SetScale(const FVector3& Scale)
	{
		SetScale(m_Scale.x, m_Scale.y, m_Scale.z);
	}

	inline FVector3 ieTransform::GetLocalUp()
	{
		RotateVector(m_LocalUp, FVector3::Up, m_RotationMat);
		return m_LocalUp;
	}
	
	inline FVector3 ieTransform::GetLocalDown()
	{
		RotateVector(m_LocalDown, FVector3::Down, m_RotationMat);
		return m_LocalDown;
	}
	
	inline FVector3 ieTransform::GetLocalLeft()
	{
		RotateVector(m_LocalLeft, FVector3::Left, m_RotationMat);
		return m_LocalLeft;
	}
	
	inline FVector3 ieTransform::GetLocalRight()
	{
		RotateVector(m_LocalRight, FVector3::Right, m_RotationMat);
		return m_LocalRight;
	}
	
	inline FVector3 ieTransform::GetLocalForward()
	{
		RotateVector(m_LocalForward, FVector3::Forward, m_RotationMat);
		return m_LocalForward;
	}
	
	inline FVector3 ieTransform::GetLocalBackward()
	{
		RotateVector(m_LocalBackward, FVector3::Backward, m_RotationMat);
		return m_LocalBackward;
	}
}
