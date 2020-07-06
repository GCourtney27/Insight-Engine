#include <ie_pch.h>

#include "Insight/Core/Application.h"
#include "Insight/Runtime/Components/Actor_Component.h"
#include "Insight/Runtime/Components/Static_Mesh_Component.h"
#include "Insight/Runtime/Components/CSharp_Scirpt_Component.h"

#include "AActor.h"

#include "imgui.h"

namespace Insight {


	AActor::AActor(ActorId Id, ActorName ActorName)
		: m_Id(Id)
	{
		SceneNode::SetDisplayName(ActorName);

	}

	AActor::~AActor()
	{
	}

	bool AActor::LoadFromJson(const rapidjson::Value& jsonActor)
	{
		if (!m_CanBeFileParsed)
			return true;

		// Load Transform
		float posX, posY, posZ;
		float rotX, rotY, rotZ;
		float scaX, scaY, scaZ;
		const rapidjson::Value& transform = jsonActor["Transform"];
		// Position
		json::get_float(transform[0], "posX", posX);
		json::get_float(transform[0], "posY", posY);
		json::get_float(transform[0], "posZ", posZ);
		SceneNode::GetTransformRef().SetPosition(Vector3(posX, posY, posZ));
		// Rotation
		json::get_float(transform[0], "rotX", rotX);
		json::get_float(transform[0], "rotY", rotY);
		json::get_float(transform[0], "rotZ", rotZ);
		SceneNode::GetTransformRef().SetRotation(Vector3(rotX, rotY, rotZ));
		// Scale
		json::get_float(transform[0], "scaX", scaX);
		json::get_float(transform[0], "scaY", scaY);
		json::get_float(transform[0], "scaZ", scaZ);
		SceneNode::GetTransformRef().SetScale(Vector3(scaX, scaY, scaZ));

		SceneNode::GetTransformRef().EditorInit();

		// Load Subobjects
		const rapidjson::Value& jsonSubobjects = jsonActor["Subobjects"];

		for (UINT i = 0; i < jsonSubobjects.Size(); ++i) {

			if (jsonSubobjects[i].HasMember("StaticMesh")) {
				StrongActorComponentPtr ptr = AActor::CreateDefaultSubobject<StaticMeshComponent>();
				ptr->LoadFromJson(jsonSubobjects[i]["StaticMesh"]);
				continue;
			}
			else if (jsonSubobjects[i].HasMember("CSharpScript")) {
				StrongActorComponentPtr ptr = AActor::CreateDefaultSubobject<CSharpScriptComponent>();
				ptr->LoadFromJson(jsonSubobjects[i]["CSharpScript"]);
				continue;
			}
		}
		return true;
	}

	bool AActor::WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer)
	{
		if (!m_CanBeFileParsed)
			return true;

		Writer.StartObject(); // Start Write Actor
		{
			Writer.Key("Type");
			Writer.String("Actor");

			Writer.Key("DisplayName");
			Writer.String(SceneNode::GetDisplayName());

			Writer.Key("Transform");
			Writer.StartArray(); // Start Write Transform
			{
				Transform& Transform = SceneNode::GetTransformRef();
				Vector3 Pos = Transform.GetPosition();
				Vector3 Rot = Transform.GetRotation();
				Vector3 Sca = Transform.GetScale();

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
			Writer.EndArray(); // End Write Transform

			Writer.Key("Subobjects");
			Writer.StartArray(); // Start Write SubObjects
			{
				for (size_t i = 0; i < m_NumComponents; ++i)
				{
					Writer.StartObject();
					m_Components[i]->WriteToJson(Writer);
					Writer.EndObject();
				}
			}
			Writer.EndArray(); // End Write SubObjects
		}
		Writer.EndObject(); // End Write Actor
		return true;
	}

	void AActor::RenderSceneHeirarchy()
	{
		ImGuiTreeNodeFlags treeFlags = m_Children.empty() ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		const bool isExpanded = ImGui::TreeNodeEx(SceneNode::GetDisplayName(), treeFlags);

		if (ImGui::IsItemClicked()) {
			IE_STRIP_FOR_GAME_DIST(Application::Get().GetEditorLayer().SetSelectedActor(this);)
		}

		if (isExpanded) {

			SceneNode::RenderSceneHeirarchy();

			for (size_t i = 0; i < m_NumComponents; ++i) {
				m_Components[i]->RenderSceneHeirarchy();
			}

			ImGui::TreePop();
			ImGui::Spacing();
		}
	}

	static int currentIndex = 0;
	void AActor::OnImGuiRender()
	{
		ImGui::Text(SceneNode::GetDisplayName());
		ImGui::Spacing();

		ImGui::Text("Transform");
		ImGui::DragFloat3("Position", &SceneNode::GetTransformRef().GetPositionRef().x, 0.05f, -100.0f, 100.0f);
		ImGui::DragFloat3("Scale", &SceneNode::GetTransformRef().GetScaleRef().x, 0.05f, -100.0f, 100.0f);
		ImGui::DragFloat3("Rotation", &SceneNode::GetTransformRef().GetRotationRef().x, 0.05f, -100.0f, 100.0f);

		for (size_t i = 0; i < m_NumComponents; ++i)
		{
			ImGui::Spacing();
			m_Components[i]->OnImGuiRender();
		}
		//ImGui::NewLine();
		//ImGui::NewLine();
		//ImGui::NewLine();
		//static const char* availableComponents[] = { "Static Mesh", "C-Sharp Script" };
		//if (ImGui::Combo("Add Component", &currentIndex, availableComponents, IM_ARRAYSIZE(availableComponents))) {
		//	switch (currentIndex) {
		//	case 0:
		//	{
		//		IE_CORE_INFO("Adding Static Mesh component to \"{0}\"", AActor::GetDisplayName());
		//		StrongActorComponentPtr ptr = AActor::CreateDefaultSubobject<StaticMeshComponent>();
		//		static_cast<StaticMeshComponent*>(ptr.get())->AttachMesh(FileSystem::Get().GetRelativeAssetDirectoryPath("Models/Sphere.obj"));
		//		//TODO:Make something like this for the material: static_cast<StaticMeshComponent*>(ptr.get())->AttachMaterial(Material::GetDefaultUntexturedMaterial());
		//		break;
		//	}
		//	case 1:
		//	{
		//		IE_CORE_INFO("Adding C-Sharp script component to \"{0}\"", AActor::GetDisplayName());
		//		StrongActorComponentPtr ptr = AActor::CreateDefaultSubobject<CSharpScriptComponent>();
		//		break;
		//	}
		//	default:
		//	{
		//		IE_CORE_INFO("Failed to determine component to add to actor \"{0}\" with index of \"{1}\"", AActor::GetDisplayName(), currentIndex);
		//		break;
		//	}
		//	}
		//}

	}

	bool AActor::OnInit()
	{
		SceneNode::OnInit();
		return true;
	}

	bool AActor::OnPostInit()
	{
		SceneNode::OnPostInit();

		return true;
	}

	void AActor::OnUpdate(const float& deltaMs)
	{
		SceneNode::OnUpdate(deltaMs);


		for (size_t i = 0; i < m_NumComponents; ++i)
		{
			m_Components[i]->OnUpdate(deltaMs);
		}
	}

	void AActor::OnPreRender(XMMATRIX parentMat)
	{
		if (m_Parent) {
			GetTransformRef().SetWorldMatrix(XMMatrixMultiply(GetTransformRef().GetLocalMatrixRef(), parentMat));
		}
		else {
			GetTransformRef().SetWorldMatrix(GetTransformRef().GetLocalMatrix());
		}

		// Render Children
		SceneNode::OnPreRender(GetTransformRef().GetWorldMatrixRef());
		// Render Components
		for (size_t i = 0; i < m_NumComponents; ++i) {
			m_Components[i]->OnPreRender(GetTransformRef().GetLocalMatrixRef());
		}
	}

	void AActor::OnRender()
	{
		// Render Children
		SceneNode::OnRender();

		// Render Components
		for (size_t i = 0; i < m_NumComponents; ++i) {
			m_Components[i]->OnRender();
		}
	}

	void AActor::BeginPlay()
	{
		SceneNode::BeginPlay();
		for (auto& comp : m_Components)
		{
			comp->BeginPlay();
		}
	}

	void AActor::Tick(const float& deltaMs)
	{
		SceneNode::Tick(deltaMs);
		for (auto& comp : m_Components)
		{
			comp->Tick(deltaMs);
		}
	}

	void AActor::Exit()
	{
		SceneNode::Exit();

	}

	void AActor::Destroy()
	{
		SceneNode::Destroy();

	}

	void AActor::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

	}

	void AActor::RemoveSubobject(StrongActorComponentPtr component)
	{
		IE_CORE_ASSERT(std::find(m_Components.begin(), m_Components.end(), component) != m_Components.end(), "Could not find Component in Actor list while attempting to delete");

		auto iter = std::find(m_Components.begin(), m_Components.end(), component);
		(*iter)->OnDetach();
		(*iter)->OnDestroy();
		(*iter).reset();
		m_Components.erase(iter);
		m_NumComponents--;
	}

}

