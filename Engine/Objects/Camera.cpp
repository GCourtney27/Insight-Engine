#include "Camera.h"
#include "..\Components\MeshRenderComponent.h"
#include "..\Editor\Editor.h"

Camera::Camera()
{
	this->m_transform.GetPosition() = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->m_transform.GetPositionVectorRef() = XMLoadFloat3(&this->m_transform.GetPosition());
	this->m_transform.GetRotation() = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->m_transform.GetRotationVectorRef() = XMLoadFloat3(&this->m_transform.GetRotation());
	this->UpdateViewMatrix();
}

bool Camera::Initialize(Scene * scene, const ID & id)
{
	this->m_scene = scene;
	this->m_id = id;

	// Set defaults for transform, needed for view matrix construct
	m_transform.SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_transform.SetPosition(XMLoadFloat3(&m_transform.GetPosition()));
	m_transform.SetRotation(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_transform.SetRotation(XMLoadFloat3(&m_transform.GetRotation()));

	this->UpdateViewMatrix();

	return true;
}

void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	m_nearZ = nearZ;
	m_farZ = farZ;
	float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

void Camera::Draw(const XMMATRIX & projectionMatrix, const XMMATRIX & viewMatrix)
{
	if (Debug::Editor::Instance()->PlayingGame())
		return;

	MeshRenderer* mr = GetComponent<MeshRenderer>();
	mr->Draw(projectionMatrix, viewMatrix);
	
}

void Camera::Update(const float& deltaTime)
{

	// If the editor is not playing keep coppying the transforms
	if (!Debug::Editor::Instance()->PlayingGame())
		UpdateTransformCopyWithTransform();

	// If editor is present do this if not just remove this
	EditorSelection* es = GetComponent<EditorSelection>();
	if (es != nullptr)
		es->SetPosition(m_transform.GetPosition());
	

	m_transform.AdjustPosition(0.0f, 0.0f, 0.0f);
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
	XMVECTOR camTarget = XMVector3TransformCoord(m_transform.GetDefaultForwardVector(), camRotationMatrix);
	// Adjust Camera target to be offset by the Camera;s current position
	camTarget += m_transform.GetPositionVectorRef();
	// Calculate up direction based on current rotation
	XMVECTOR upDir = XMVector3TransformCoord(m_transform.GetDefaultUpVector(), camRotationMatrix);
	// Rebuid view Matrix 
	m_viewMatrix = XMMatrixLookAtLH(m_transform.GetPositionVectorRef(), camTarget, upDir);

	m_transform.UpdateDirectionVectors();
}
