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
		AttachMesh(modelPath);

		// Load Material
		m_Material.LoadFromJson(jsonStaticMeshComponent[1]);

		return true;
	}

	bool StaticMeshComponent::WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer)
	{
		Writer.Key("StaticMesh");
		Writer.StartArray(); // Start SM Write
		{
			// Core Attributes
			Writer.StartObject(); // Start Mesh Directory
			{
				Writer.Key("Mesh");
				Writer.String(m_pModel->GetAssetDirectoryRelativePath().c_str());
				Writer.Key("Enabled");
				Writer.Bool(ActorComponent::m_Enabled);
			}
			Writer.EndObject(); // End Mesh Directory

			// Material
			Writer.StartObject(); // Start Material Attributes
			{
				m_Material.WriteToJson(Writer);
			}
			Writer.EndObject(); // End Material Attributes

		}
		Writer.EndArray(); // End SM Write

		return true;
	}

	void StaticMeshComponent::OnInit()
	{

	}

	void StaticMeshComponent::OnDestroy()
	{
	}

	void StaticMeshComponent::CalculateParent(const XMMATRIX& parentMatrix)
	{
		//if (m_ModelLoadFuture.get()) {
			m_pModel->PreRender(parentMatrix);
		//}
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

	static std::mutex s_MeshMutex;
	static bool LoadMesh(StrongModelPtr Model, const std::string& Path, Material* Material)
	{
		Model->Init(Path, Material);

		std::lock_guard<std::mutex> ResourceLock(s_MeshMutex);

		ResourceManager::Get().GetModelManager().RegisterModel(Model);
		return true;
	}

	void StaticMeshComponent::AttachMesh(const std::string& AssesDirectoryRelPath)
	{
		ScopedTimer timer(("StaticMeshComponent::AttachMesh \"" + AssesDirectoryRelPath + "\"").c_str());

		if (m_pModel) {
			m_pModel.reset();
		}

		m_pModel = make_shared<Model>();
		m_pModel->Init(AssesDirectoryRelPath, &m_Material);
		//m_ModelLoadFuture = std::async(std::launch::async, LoadMesh, m_pModel, AssesDirectoryRelPath, &m_Material);
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
