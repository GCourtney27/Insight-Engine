#include "MeshRenderComponent.h"
#include "..\\ErrorLogger.h"
#include "..\Graphics\ImGui\imgui.h"
#include "..\Graphics\Graphics.h"

void MeshRenderer::InitFromJSON(Entity* owner, const rapidjson::Value& componentInformation)
{
	std::string model_FilePath;
	std::string materialType;
	std::string albedo_Filepath;
	std::string normal_Filepath;
	std::string metallic_Filepath;
	std::string roughness_Filepath;
	std::vector<std::string> textures;


	json::get_string(componentInformation[0], "Model", model_FilePath);

	json::get_string(componentInformation[1], "MaterialType", materialType);

	json::get_string(componentInformation[1], "Albedo", albedo_Filepath);
	json::get_string(componentInformation[1], "Normal", normal_Filepath);
	json::get_string(componentInformation[1], "Metallic", metallic_Filepath);
	json::get_string(componentInformation[1], "Roughness", roughness_Filepath);
	

	textures.push_back(albedo_Filepath);
	textures.push_back(normal_Filepath);
	textures.push_back(metallic_Filepath);
	textures.push_back(roughness_Filepath);
	
	Material* pMaterial = nullptr;
	//pMaterial->SetMaterialByType().Initialize(device, context, eMaterialType) // inside of this->Initialize() below

	this->Initialize(owner, model_FilePath,
					Graphics::Instance()->GetDevice(), 
					Graphics::Instance()->GetDeviceContext(),
					Graphics::Instance()->GetDefaultVertexShader(), 
					new Material(Graphics::Instance()->GetDevice(), Graphics::Instance()->GetDeviceContext(),
								Material::GetMaterialTypeFromString(materialType), Material::eFlags::NOFLAGS, textures));


	/*MeshRenderer* finalMesh = nullptr;
	bool foundModel = false;
	std::string model_FilePath;
	std::string materialType;
	std::string albedo_Filepath;
	std::string normal_Filepath;
	std::string metallic_Filepath;
	std::string roughness_Filepath;
	std::vector<std::string> textures;
	for (rapidjson::SizeType m = 0; m < meshRenderer.Size(); m++)
	{
		json::get_string(meshRenderer[m], "Model", model_FilePath);
		if (model_FilePath != "NONE" && !foundModel)
		{
			foundModel = true;
			finalMesh = entity->AddComponent<MeshRenderer>();
		}
		json::get_string(meshRenderer[m], "MaterialType", materialType);
		json::get_string(meshRenderer[m], "Albedo", albedo_Filepath);
		json::get_string(meshRenderer[m], "Normal", normal_Filepath);
		json::get_string(meshRenderer[m], "Metallic", metallic_Filepath);
		json::get_string(meshRenderer[m], "Roughness", roughness_Filepath);
	}
	if (finalMesh != nullptr)
	{
		textures.push_back(albedo_Filepath);
		textures.push_back(normal_Filepath);
		textures.push_back(metallic_Filepath);
		textures.push_back(roughness_Filepath);
		//
		finalMesh->Initialize(entity, model_FilePath, device, deviceContext, cb_vs_vertexshader, new Material(device, deviceContext, materialType, textures));
	}*/

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

}


