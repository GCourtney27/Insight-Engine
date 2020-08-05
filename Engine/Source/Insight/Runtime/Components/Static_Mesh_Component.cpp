#include <ie_pch.h>

#include "Static_Mesh_Component.h"

#include "Insight/Runtime/AActor.h"
#include "Insight/Systems/File_System.h"
#include "Insight/Systems/Managers/Resource_Manager.h"
#include "Insight/Rendering/Renderer.h"
#include "Insight/Rendering/Material.h"


#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace Insight {

	uint32_t StaticMeshComponent::s_NumActiveSMComponents = 0U;

	StaticMeshComponent::StaticMeshComponent(AActor* pOwner)
		: ActorComponent("Static Mesh Component", pOwner)
	{
		m_pMaterial = new Material();
	}

	StaticMeshComponent::~StaticMeshComponent()
	{
	}

	bool StaticMeshComponent::LoadFromJson(const rapidjson::Value& JsonStaticMeshComponent)
	{
		// Load Material
		m_pMaterial->LoadFromJson(JsonStaticMeshComponent[1]);

		// Load Mesh
		std::string ModelPath;
		json::get_string(JsonStaticMeshComponent[0], "Mesh", ModelPath);
		AttachMesh(ModelPath);

		json::get_bool(JsonStaticMeshComponent[0], "Enabled", ActorComponent::m_Enabled);
		
		// Load Mesh Local Transform
		float posX, posY, posZ;
		float rotX, rotY, rotZ;
		float scaX, scaY, scaZ;
		const rapidjson::Value& MeshTransform = JsonStaticMeshComponent[0]["LocalTransform"];
		// Position
		json::get_float(MeshTransform[0], "posX", posX);
		json::get_float(MeshTransform[0], "posY", posY);
		json::get_float(MeshTransform[0], "posZ", posZ);
		m_pModel->GetMeshRootTransformRef().SetPosition(ieVector3(posX, posY, posZ));
		// Rotation
		json::get_float(MeshTransform[0], "rotX", rotX);
		json::get_float(MeshTransform[0], "rotY", rotY);
		json::get_float(MeshTransform[0], "rotZ", rotZ);
		m_pModel->GetMeshRootTransformRef().SetRotation(ieVector3(rotX, rotY, rotZ));
		// Scale
		json::get_float(MeshTransform[0], "scaX", scaX);
		json::get_float(MeshTransform[0], "scaY", scaY);
		json::get_float(MeshTransform[0], "scaZ", scaZ);
		m_pModel->GetMeshRootTransformRef().SetScale(ieVector3(scaX, scaY, scaZ));

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
				Writer.Key("LocalTransform");
				Writer.StartArray();
				{
					ieTransform& MeshTransform = m_pModel->GetMeshRootTransformRef();
					ieVector3 Pos = MeshTransform.GetPosition();
					ieVector3 Rot = MeshTransform.GetRotation();
					ieVector3 Sca = MeshTransform.GetScale();
					Writer.StartObject();
					// Position
					Writer.Key("posX");
					Writer.Double(Pos.x);
					Writer.Key("posY");
					Writer.Double(Pos.y);
					Writer.Key("posZ");
					Writer.Double(Pos.z);
					// Rotation
					Writer.Key("rotX");
					Writer.Double(Rot.x);
					Writer.Key("rotY");
					Writer.Double(Rot.y);
					Writer.Key("rotZ");
					Writer.Double(Rot.z);
					// Scale
					Writer.Key("scaX");
					Writer.Double(Sca.x);
					Writer.Key("scaY");
					Writer.Double(Sca.y);
					Writer.Key("scaZ");
					Writer.Double(Sca.z);
					Writer.EndObject();
				}
				Writer.EndArray();
			}
			Writer.EndObject(); // End Mesh Directory

			// Material
			Writer.StartObject(); // Start Material Attributes
			{
				m_pMaterial->WriteToJson(Writer);
			}
			Writer.EndObject(); // End Material Attributes

		}
		Writer.EndArray(); // End SM Write

		return true;
	}

	void StaticMeshComponent::OnEvent(Event& e)
	{
	}

	void StaticMeshComponent::OnInit()
	{

	}

	void StaticMeshComponent::OnDestroy()
	{
		GeometryManager::UnRegisterOpaqueModel(m_pModel);
		m_pModel->Destroy();
		delete m_pMaterial;
	}

	void StaticMeshComponent::CalculateParent(const XMMATRIX& parentMatrix)
	{
		//if (m_ModelLoadFuture.get()) {
			m_pModel->CalculateParent(parentMatrix);
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
		ImGui::PushID(m_IDBuffer);
		
		if (ImGui::CollapsingHeader(m_ComponentName, ImGuiTreeNodeFlags_DefaultOpen)) {

			if (ImGui::InputText("New Mesh Dir: ", &m_DynamicAssetDir, ImGuiInputTextFlags_EnterReturnsTrue)) {
				if (FileSystem::FileExists(m_DynamicAssetDir)) {
					AttachMesh(m_DynamicAssetDir);
				}
				else {
					IE_CORE_ERROR("File does not exist with path: \"{0}\"", m_DynamicAssetDir);
				}
			}

			m_pModel->OnImGuiRender();
			m_pMaterial->OnImGuiRender();
		}
		ImGui::PopID();
	}

	void StaticMeshComponent::RenderSceneHeirarchy()
	{
	}

	static std::mutex s_MeshMutex;
	static bool LoadModelAsync(StrongModelPtr Model, const std::string& Path, Material* Material)
	{
		Model->Create(Path, Material);

		std::lock_guard<std::mutex> ResourceLock(s_MeshMutex);

		GeometryManager::RegisterOpaqueModel(Model);
		return true;
	}

	void StaticMeshComponent::AttachMesh(const std::string& AssestDirectoryRelPath)
	{
		Profiling::ScopedTimer timer(("StaticMeshComponent::AttachMesh \"" + AssestDirectoryRelPath + "\"").c_str());

		if (m_pModel) {
			GeometryManager::UnRegisterOpaqueModel(m_pModel);
			m_pModel->Destroy();
			m_pModel.reset();
		}
		m_pModel = make_shared<Model>();
		m_pModel->Create(AssestDirectoryRelPath, m_pMaterial);

		Material::eMaterialType MaterialType = m_pMaterial->GetMaterialType();
		
		if (MaterialType == Material::eMaterialType::eMaterialType_Opaque) {
			GeometryManager::RegisterOpaqueModel(m_pModel);
		}
		else if (MaterialType == Material::eMaterialType::eMaterialType_Translucent) {
			GeometryManager::RegisterTranslucentModel(m_pModel);
		}

		// Experamental: Multi-threaded model laoding
		//m_ModelLoadFuture = std::async(std::launch::async, LoadModelAsync, m_pModel, AssestDirectoryRelPath, m_pMaterial);
	}

	void StaticMeshComponent::SetMaterial(Material* pMaterial)
	{
		if (m_pMaterial) {
			delete m_pMaterial;
			m_pMaterial = nullptr;
		}
		m_pMaterial = pMaterial;
	}

	void StaticMeshComponent::BeginPlay()
	{
	}

	void StaticMeshComponent::Tick(const float DeltaMs)
	{
	}

	void StaticMeshComponent::OnAttach()
	{
		m_SMWorldIndex = s_NumActiveSMComponents++;
		sprintf_s(m_IDBuffer, "SM-%u", m_SMWorldIndex);
	}

	void StaticMeshComponent::OnDetach()
	{
		s_NumActiveSMComponents--;
		GeometryManager::UnRegisterOpaqueModel(m_pModel);
	}

}
