#include "Camera.h"
#include "..\Components\MeshRenderComponent.h"
#include "..\Editor\Editor.h"

bool Camera::Initialize(Scene * scene, const ID & id)
{
	return true;
}

void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	m_nearZ = nearZ;
	m_farZ = farZ;
	float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

void Camera::Draw(const XMMATRIX & viewProjectionMatrix, const XMMATRIX & viewMatrix)
{
	if (Debug::Editor::Instance()->PlayingGame())
		return;

	MeshRenderer* mr = GetComponent<MeshRenderer>();

	if (mr != nullptr)
	{
		mr->Draw(viewProjectionMatrix, viewMatrix);
	}
}

void Camera::Update(const float& deltaTime)
{

	// If the editor is not playing keep coppying the transforms
	if (!Debug::Editor::Instance()->PlayingGame())
		UpdateTransformCopyWithTransform();

	// If editor is present do this if not just remove this
	EditorSelection* es = GetComponent<EditorSelection>();
	if (es != nullptr)
	{
		es->SetPosition(m_transform.GetPosition());
	}

	//m_transform.AdjustPosition(0.0f, 0.0f, 0.0f);
	this->m_transform.Update();
	UpdateViewMatrix();
}

const XMMATRIX & Camera::GetViewMatrix()
{
	return m_viewMatrix;
}

const XMMATRIX & Camera::GetProjectionMatrix()
{
	return m_projectionMatrix;
}

void Camera::UpdateViewMatrix()
{
	// Calculate Camera rotation matrix
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(m_transform.GetRotation().x, m_transform.GetRotation().y, m_transform.GetRotation().z);
	// Calculate unit vector of Camera target based off of Camera forward value transformed by Camera rotation
	XMVECTOR camTarget = XMVector3TransformCoord(m_transform.GetForwardVector(), camRotationMatrix);
	// Adjust Camera target to be offset by the Camera;s current position
	camTarget += m_transform.GetPositionVector();
	// Calculate up direction based on current rotation
	XMVECTOR upDir = XMVector3TransformCoord(m_transform.GetUpVector(), camRotationMatrix);
	// Rebuid view Matrix
	m_viewMatrix = XMMatrixLookAtLH(m_transform.GetPositionVector(), camTarget, upDir);

	m_transform.UpdateDirectionVectors();
}
