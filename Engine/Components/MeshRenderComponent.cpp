#include "MeshRenderComponent.h"
#include "..\\ErrorLogger.h"
bool MeshRenderer::Initialize(const std::string & filepath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	if (!model.Initialize(filepath, device, deviceContext, cb_vs_vertexshader))
	{
		std::string error = "Failed to Initialize model for Entity";
		ErrorLogger::Log(error);
		return false;
	}
	m_pName = "MeshRenderer";

	return true;
}

void MeshRenderer::Draw(const XMMATRIX & viewProjectionMatrix)
{
	model.Draw(this->worldMatrix, viewProjectionMatrix);
}

void MeshRenderer::Update()
{
}

void MeshRenderer::Destroy()
{
}
