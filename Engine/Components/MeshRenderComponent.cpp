#include "MeshRenderComponent.h"
#include "..\\ErrorLogger.h"
#include "..\Graphics\ImGui\imgui.h"
#include "..\Graphics\Graphics.h"

void MeshRenderer::InitFromJSON(Entity* owner, const rapidjson::Value& componentInformation)
{
	std::string model_FilePath;
	std::string materialType;

	json::get_string(componentInformation[0], "Model", model_FilePath);
	json::get_string(componentInformation[1], "MaterialType", materialType);
	
	Material* pMaterial = nullptr;
	pMaterial = pMaterial->SetMaterialByType(Material::GetMaterialTypeFromString(materialType));
	
	pMaterial->Initiailze(Graphics::Instance()->GetDevice(), Graphics::Instance()->GetDeviceContext(), componentInformation, Material::eFlags::NOFLAGS);

	this->Initialize(owner, model_FilePath,
					Graphics::Instance()->GetDevice(),
					Graphics::Instance()->GetDeviceContext(),
					Graphics::Instance()->GetDefaultVertexShader(),
					pMaterial);

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
	static bool isComponentEnabled = GetIsDrawEnabled();
	if (ImGui::Checkbox("Is Enabled", &isComponentEnabled))
	{
		this->SetIsDrawEnabled(isComponentEnabled);
	}

	ImGui::DragFloat3("Color", &model.GetMaterial()->m_color.x, 0.1f, 0.0f, 255.0f);
	ImGui::DragFloat("Metallic", &model.GetMaterial()->m_metallic, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Roughness", &model.GetMaterial()->m_roughness, 0.01f, 0.0f, 1.0f);

	model.GetMaterial()->cb_ps_perObjectColor.data.color = model.GetMaterial()->m_color;
	model.GetMaterial()->cb_ps_perObjectColor.data.metallic = model.GetMaterial()->m_metallic;
	model.GetMaterial()->cb_ps_perObjectColor.data.roughnss = model.GetMaterial()->m_roughness;
	model.GetMaterial()->cb_ps_perObjectColor.ApplyChanges();
	
}


