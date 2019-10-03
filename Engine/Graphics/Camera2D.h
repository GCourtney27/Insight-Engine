#pragma once
#include "GameObject.h"

class Camera2D : public GameObject
{
public:
	Camera2D();
	void  SetProjectionValues(float width, float height, float nearX, float farZ);

	const XMMATRIX & GetOrthoMatrix() const;
	const XMMATRIX & GetWorldmatrix() const;
private:
	void UpdateMatrix() override;

	XMMATRIX orthoMatrix;
	XMMATRIX worldMatrix;
};