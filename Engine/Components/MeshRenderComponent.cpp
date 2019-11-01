#include "MeshRenderComponent.h"
#include "..\\ErrorLogger.h"
#include "..\Graphics\ImGui\imgui.h"
#include "..\Graphics\Graphics.h"
#include "..\Editor\Editor.h"
#include "..\Graphics\MaterialTextured.h"

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
		model.Draw(m_owner->GetTransform().GetWorldMatrix(), projectionMatrix, viewMatrix);
}

void MeshRenderer::Update(const float& deltaTime)
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

	if (model.GetMaterial() != nullptr)
	{
		Material* mat = model.GetMaterial();

		ImGui::Text(model.GetMaterial()->GetMaterialTypeAsString().c_str());

		ImGui::DragFloat3("Color", &model.GetMaterial()->m_color.x, 0.1f, 0.0f, 1.0f);
		ImGui::DragFloat("Metallic", &model.GetMaterial()->m_metallic, 0.01f, -1.0f, 1.0f);
		ImGui::DragFloat("Roughness", &model.GetMaterial()->m_roughness, 0.01f, -1.0f, 1.0f);

		model.GetMaterial()->cb_ps_perObjectColor.data.color = model.GetMaterial()->m_color;
		model.GetMaterial()->cb_ps_perObjectColor.data.metallic = model.GetMaterial()->m_metallic;
		model.GetMaterial()->cb_ps_perObjectColor.data.roughnss = model.GetMaterial()->m_roughness;
		model.GetMaterial()->cb_ps_perObjectColor.ApplyChanges();

		//for (int i = 0; i < mat->m_textures.size(); i++)
		//{
		//	dynamic_cast<MaterialTextured*>(mat);
		//	if (mat != nullptr)
		//	{
		//		dynamic_cast<MaterialTextured*>(mat)->GetTextureLocations();
		//		DEBUGLOG("hi");
		//	}
		//	//DEBUGLOG(StringHelper::GetFilenameFromDirectory(mat->m_textures[i]));
		//}
		//StringHelper::GetFilenameFromDirectory()
	}
	
	
}


