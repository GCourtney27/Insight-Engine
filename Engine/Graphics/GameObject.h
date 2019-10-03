#pragma once
#include "Model.h"

class GameObject
{
public:
	
	void Initialize();

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

	std::string GetName() const { return m_name; }
	void SetName(std::string value) { m_name = value; }

	XMFLOAT3 & GetPosition() { return this->pos; }
	XMFLOAT3 & GetRotation() { return this->rot; }
	XMFLOAT3 & GetScale() { return this->scale; }

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

	std::string m_name = "";


};