#include <ie_pch.h>

#include "Insight/Runtime/AActor.h"
#include "Static_Mesh_Component.h"
#include "Insight/Systems/File_System.h"
#include "Insight/Systems/Managers/Resource_Manager.h"
#include <imgui.h>

namespace Insight {



	StaticMeshComponent::StaticMeshComponent(AActor* pOwner)
		: ActorComponent("Static Mesh Component", pOwner)
	{

	}

	StaticMeshComponent::~StaticMeshComponent()
	{
	}

	bool StaticMeshComponent::LoadFromJson(const rapidjson::Value& jsonStaticMeshComponent)
	{
		// Load Mesh
		std::string modelPath;
		json::get_string(jsonStaticMeshComponent[0], "Mesh", modelPath);
		json::get_bool(jsonStaticMeshComponent[0], "Enabled", ActorComponent::m_Enabled);
		AttachMesh(FileSystem::Get().GetRelativeAssetDirectoryPath(modelPath));

		// Load Material
		m_Material.LoadFromJson(jsonStaticMeshComponent[1]);

		return true;
	}

	void StaticMeshComponent::OnInit()
	{

	}

	void StaticMeshComponent::OnDestroy()
	{
	}

	void StaticMeshComponent::OnPreRender(const XMMATRIX& parentMatrix)
	{
		m_pModel->PreRender(parentMatrix);
	}

	void StaticMeshComponent::OnRender()
	{
	}

	void StaticMeshComponent::OnUpdate(const float& deltaTime)
	{
	}

	void StaticMeshComponent::OnImGuiRender()
	{
		ImGui::Spacing();
		if (ImGui::CollapsingHeader(m_ComponentName, ImGuiTreeNodeFlags_DefaultOpen)) {

			m_pModel->OnImGuiRender();
			m_Material.OnImGuiRender();
		}
	}

	void StaticMeshComponent::RenderSceneHeirarchy()
	{
	}

	void StaticMeshComponent::AttachMesh(const std::string& path)
	{
		ScopedTimer timer(("StaticMeshComponent::AttachMesh \"" + path + "\"").c_str());

		if (m_pModel) {
			m_pModel.reset();
		}
		m_pModel = make_shared<Model>(path, &m_Material);
		ResourceManager::Get().GetModelManager().RegisterModel(m_pModel);
	}

	void StaticMeshComponent::BeginPlay()
	{
	}

	void StaticMeshComponent::Tick(const float& deltaMs)
	{
	}

	void StaticMeshComponent::OnAttach()
	{
	}

	void StaticMeshComponent::OnDetach()
	{
	}

}
