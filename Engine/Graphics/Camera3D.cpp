#include "Camera3D.h"

Camera3D::Camera3D()
{
	this->pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->posVector = XMLoadFloat3(&this->pos);
	this->rot = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}

void Camera3D::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	m_nearZ = nearZ;
	m_farZ = farZ;
	float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
	this->projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);;
}

const XMMATRIX & Camera3D::GetViewMatrix() const
{
	return this->viewMatrix;
}

const XMMATRIX & Camera3D::GetProjectionMatrix() const
{
	return this->projectionMatrix;
}

float * Camera3D::GetViewMatAsFloatArr()
{
		/*
		float result[16] =
	{ temp._11, temp._12, temp._13, temp._14,
		temp._21, temp._22, temp._23, temp._24,
		temp._31, temp._32, temp._33, temp._34,
		temp._41, temp._42, temp._43, temp._44
	};
	*/

	XMFLOAT4X4 temp;
	XMStoreFloat4x4(&temp, GetViewMatrix());


	float result[16] =
	{
		temp._11, temp._21, temp._31, temp._41,
		temp._12, temp._22, temp._32, temp._42,
		temp._13, temp._23, temp._33, temp._43,
		temp._14, temp._24, temp._34, temp._44
	};

	viewFloatMat = result;
	return viewFloatMat;
}

float * Camera3D::GetProjMatAsFloatArr()
{

	XMFLOAT4X4 temp;
	XMStoreFloat4x4(&temp, GetProjectionMatrix());

	//float result[16] =
	//{ 
	//	temp._11, temp._12, temp._13, temp._14,
	//	temp._21, temp._22, temp._23, temp._24,
	//	temp._31, temp._32, temp._33, temp._34,
	//	temp._41, temp._42, temp._43, temp._44
	//};

	float result[16] =
	{
		temp._11, temp._21, temp._31, temp._41,
		temp._12, temp._22, temp._32, temp._42,
		temp._13, temp._23, temp._33, temp._43,
		temp._14, temp._24, temp._34, temp._44
	};

	projFloatMat = result;

	return projFloatMat;
}

void Camera3D::UpdateMatrix() // Update the view matrix and also updates the movement vectors
{
	// Calculate Camera3D rotation matrix
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z);
	// Calculate unit vector of Camera3D target based off Camera3D forward value transformed by Camera3D rotation
	XMVECTOR camTarget = XMVector3TransformCoord(this->DEFAULT_FOREWARD_VECTOR, camRotationMatrix);
	// Adjust Camera3D target to be offset by the Camera3D's current position
	camTarget += this->posVector;
	// Calculate up direction based on current rotation
	XMVECTOR upDir = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, camRotationMatrix);
	// Rebuild view Matrix
	this->viewMatrix = XMMatrixLookAtLH(this->posVector, camTarget, upDir);

	this->UpdateDirectionVectors();
}
