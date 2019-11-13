#include "MeshRenderComponent.h"
#include "..\\ErrorLogger.h"
#include "..\Graphics\ImGui\imgui.h"
#include "..\Graphics\Graphics.h"
#include "..\Editor\Editor.h"
#include "..\Graphics\MaterialTextured.h"
#include <math.h>

void MeshRenderer::InitFromJSON(Entity* owner, const rapidjson::Value& componentInformation)
{

	std::string model_FilePath;
	std::string materialType;
	std::string materialFlags;

	json::get_string(componentInformation[0], "Model", model_FilePath);
	json::get_string(componentInformation[1], "MaterialType", materialType);
	json::get_string(componentInformation[1], "MaterialFlags", materialFlags);
	
	Material* pMaterial = nullptr;
	pMaterial = pMaterial->SetMaterialByType(Material::GetMaterialTypeFromString(materialType), Material::GetMaterialFlagsFromString(materialFlags));
	
	pMaterial->Initiailze(Graphics::Instance()->GetDevice(), Graphics::Instance()->GetDeviceContext(), componentInformation, Material::GetMaterialFlagsFromString(materialFlags));

	this->Initialize(owner, model_FilePath,
					Graphics::Instance()->GetDevice(),
					Graphics::Instance()->GetDeviceContext(),
					Graphics::Instance()->GetDefaultVertexShader(),
					pMaterial);

}

void MeshRenderer::WriteToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.Key("MeshRenderer");
	writer.StartArray(); // Start Mesh Renderer

	writer.StartObject(); // Start Model
	writer.Key("Model");
	writer.String(model.GetModelDirectory().c_str());
	writer.EndObject(); // End model

	writer.StartObject(); // Start Material Information
	model.GetMaterial()->WriteToJSON(writer);
	writer.EndObject(); // End Material Information

	writer.EndArray(); // End Mesh Renderer
}

bool MeshRenderer::Initialize(Entity* owner, const std::string & filepath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_PerObject>& CB_VS_PerObject, Material * material)
{
	this->m_owner = owner;

	if (!model.Initialize(filepath, device, deviceContext, CB_VS_PerObject, material))
	{
		std::string error = "Failed to Initialize model";
		//error += std::string(this->GetName());
		ErrorLogger::Log(error);
		return false;
	}
	SetName("Mesh Renderer");

	return true;
}

void MeshRenderer::Draw(const XMMATRIX & projectionMatrix, const XMMATRIX & viewMatrix)
{

	if(GetIsDrawEnabled())
		model.Draw(m_owner->GetTransform().GetWorldMatrix(), projectionMatrix, viewMatrix);
}

void MeshRenderer::Start()
{
}

void MeshRenderer::Update(const float& deltaTime)
{
	m_deltaTime = deltaTime;
}

void MeshRenderer::Destroy()
{
}

void MeshRenderer::OnImGuiRender()
{
	ImGui::Text(GetName());
	static bool isComponentEnabled = GetIsDrawEnabled();
	if (ImGui::Checkbox("Is Draw Enabled", &isComponentEnabled))
	{
		this->SetIsDrawEnabled(isComponentEnabled);
	}

	if (model.GetMaterial() != nullptr)
	{
		Material* pMat = model.GetMaterial();

		ImGui::Text("Material Type: ");
		ImGui::SameLine();
		ImGui::Text(pMat->GetMaterialTypeAsString().c_str());

		ImGui::DragFloat3("Color", &pMat->m_color.x, 0.1f, 0.0f, 255.0f);
		ImGui::DragFloat("Metallic", &pMat->m_metallic, 0.01f, -1.0f, 1.0f);
		ImGui::DragFloat("Roughness", &pMat->m_roughness, 0.01f, -1.0f, 1.0f);

		pMat->m_cb_ps_PerObjectUtil.data.color = pMat->m_color;
		pMat->m_cb_ps_PerObjectUtil.data.metallic = pMat->m_metallic;
		pMat->m_cb_ps_PerObjectUtil.data.roughness = pMat->m_roughness;
		pMat->m_cb_ps_PerObjectUtil.ApplyChanges();

		ImGui::NewLine();
		ImGui::DragFloat("UV Offset X", &pMat->m_newUVOffset.x, 0.01f, -100.0f, 100.0f);
		ImGui::DragFloat("UV Offset Y", &pMat->m_newUVOffset.y, 0.01f, -100.0f, 100.0f);

		ImGui::DragFloat("Tiling X", &pMat->m_tiling.x, 0.01f, -100.0f, 100.0f);
		ImGui::DragFloat("Tiling Y", &pMat->m_tiling.y, 0.01f, -100.0f, 100.0f);

		//ImGui::DragFloat3("Vertex Offset", &pMat->m_newVertOffset.x, 0.01f, -100.0f, 100.0f);


		//pMat->m_newUVOffset.x += 0.05f * m_deltaTime;
		//pMat->m_newUVOffset.y += 0.05f * m_deltaTime;
		
		//pMat->m_newVertOffset.x = Graphics::Instance()->GetEngineInstance()->GetFrameTimer().seconds();

		//pMat->m_cb_vs_PerObjectUtil.data.vertOffset = pMat->m_newVertOffset;
		pMat->m_cb_vs_PerObjectUtil.data.tiling = pMat->m_tiling;
		pMat->m_cb_vs_PerObjectUtil.data.uvOffset = pMat->m_newUVOffset;

		pMat->m_cb_vs_PerObjectUtil.ApplyChanges();

	}
	
	
}


