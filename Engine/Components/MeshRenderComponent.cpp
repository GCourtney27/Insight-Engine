#include "MeshRenderComponent.h"
#include "..\\ErrorLogger.h"
#include "..\Graphics\ImGui\imgui.h"

void MeshRenderer::JSONLoad()
{
}

bool MeshRenderer::Initialize(Entity* owner, const std::string & filepath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, Material * material)
{
	this->m_owner = owner;

	if (!model.Initialize(filepath, device, deviceContext, cb_vs_vertexshader, material))
	{
		std::string error = "Failed to Initialize model for: ";
		error += this->GetName();
		ErrorLogger::Log(error);
		return false;
	}
	SetName("MeshRenderer");

	return true;
}

void MeshRenderer::Draw(const XMMATRIX & projectionMatrix, const XMMATRIX & viewMatrix)
{

	if(GetIsDrawEnabled())
		model.Draw(this->worldMatrix, projectionMatrix, viewMatrix);
}

void MeshRenderer::Update(float deltaTime)
{
}

void MeshRenderer::Destroy()
{
}

void MeshRenderer::OnImGuiRender()
{
	ImGui::Text(GetName());
	//bool isComponentEnabled = GetIsEnabled();
	//ImGui::Checkbox("Is Enabled", &isComponentEnabled);

}

