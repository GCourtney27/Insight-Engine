#pragma once
#include "../Graphics/Model.h"

class Transform
{
public:

	const XMVECTOR & GetPositionVector() const;
	const DirectX::XMFLOAT3 & GetPositionFloat3() const;
	const XMVECTOR & GetRotationVector() const;
	const DirectX::XMFLOAT3 & GetRotationFloat3() const;

	void SetPosition(const XMVECTOR & pos);
	void SetPosition(const DirectX::XMFLOAT3 & pos);
	void SetPosition(float x, float y, float z);
	void AdjustPosition(const XMVECTOR & pos);
	void AdjustPosition(const DirectX::XMFLOAT3 & pos);
	void AdjustPosition(float x, float y, float z);

	void SetRotation(const XMVECTOR & rot);
	void SetRotation(const DirectX::XMFLOAT3 & rot);
	void SetRotation(float x, float y, float z);
	void AdjustRotation(const XMVECTOR & rot);
	void AdjustRotation(const DirectX::XMFLOAT3 & rot);
	void AdjustRotation(float x, float y, float z);

	void SetScale(const XMVECTOR & scale);
	void SetScale(const DirectX::XMFLOAT3 & scale);
	void SetScale(float xScale, float yScale, float zScale = 1.0f);
	void AdjustScale(const XMVECTOR & scale);
	void AdjustScale(const DirectX::XMFLOAT3 & scale);
	void AdjustScale(float x, float y, float z);

	XMFLOAT3 & GetPosition() { return this->pos; }
	XMFLOAT3 & GetRotation() { return this->rot; }
	XMFLOAT3 & GetScale() { return this->scale; }
	

	void SetLookAtPos(DirectX::XMFLOAT3 lookAtPos);
	const XMVECTOR & GetForwardVector(bool omitY = false);
	const XMVECTOR & GetBackwardVector(bool omitY = false);
	const XMVECTOR & GetLeftVector(bool omitY = false);
	const XMVECTOR & GetRightVector(bool omitY = false);
	const XMVECTOR & GetUpVector();
	const XMVECTOR & GetDownVector();

protected:
	virtual void UpdateMatrix();

	// XMVECTOR
	XMVECTOR posVector; // position in the world
	XMVECTOR rotVector; // rotation in the world
	XMVECTOR scaleVector; // scale in the world
	// DirectX::XMFLOAT3
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 rot;
	DirectX::XMFLOAT3 scale;

	void UpdateDirectionVectors();

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