#include <ie_pch.h>

#include "Insight/Runtime/AActor.h"
#include "Static_Mesh_Component.h"
#include "Insight/Systems/Model_Manager.h"
#include "Insight/Systems/File_System.h"

#include "imgui.h"

namespace Insight {



	StaticMeshComponent::StaticMeshComponent(StrongActorPtr pOwner)
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
		AttachMesh(FileSystem::Get().GetRelativeAssetDirectoryPath(modelPath));
		m_pModel->LoadFromJson(jsonStaticMeshComponent[1]);
		// Load Material
		//m_Material.LoadFromJson(jsonStaticMeshComponent[1]);

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
		//m_pModel->Render();
		//m_pModel->Draw();
	}

	void StaticMeshComponent::OnUpdate(const float& deltaTime)
	{
	}

	void StaticMeshComponent::OnImGuiRender()
	{
		m_pModel->OnImGuiRender();

	}

	void StaticMeshComponent::RenderSceneHeirarchy()
	{
		m_pModel->RenderSceneHeirarchy();
	}

	void StaticMeshComponent::AttachMesh(const std::string& path)
	{
		if (m_pModel)
			m_pModel.reset();

		m_pModel = make_shared<Model>(path);
		ModelManager::Get().PushModel(m_pModel);
	}

	void StaticMeshComponent::OnAttach()
	{
	}

	void StaticMeshComponent::OnDetach()
	{
	}

}
