#include "MeshRenderComponent.h"
#include "..\\ErrorLogger.h"
#include "..\Graphics\ImGui\imgui.h"

void MeshRenderer::JSONLoad()
{
}

bool MeshRenderer::Initialize(Entity* owner, const std::string & filepath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	this->m_owner = owner;
	if (!model.Initialize(filepath, device, deviceContext, cb_vs_vertexshader))
	{
		std::string error = "Failed to Initialize model for Entity";
		ErrorLogger::Log(error);
		return false;
	}
	SetName("MeshRenderer");

	return true;
}

//bool MeshRenderer::Initialize(const std::string & filepath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, Material * material)
//{
//	if (!model.Initialize(filepath, device, deviceContext, cb_vs_vertexshader))
//	{
//		std::string error = "Failed to Initialize model for Entity";
//		ErrorLogger::Log(error);
//		return false;
//	}
//	m_pName = "MeshRenderer";
//	
//	m_pMaterial = material;
//
//	return true;
//}

void MeshRenderer::Draw(const XMMATRIX & viewProjectionMatrix)
{
	if(GetIsDrawEnabled())
		model.Draw(this->worldMatrix, viewProjectionMatrix);
}

void MeshRenderer::Update()
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


