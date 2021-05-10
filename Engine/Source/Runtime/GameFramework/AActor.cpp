#include <Engine_pch.h>

#include "AActor.h"

#include "Runtime/Core/Public/Application.h"
#include "Runtime/GameFramework/Components/ActorComponent.h"
#include "Runtime/GameFramework/Components/StaticMeshComponent.h"
#include "Runtime/GameFramework/Components/CSharpScirptComponent.h"
#include "Runtime/GameFramework/Components/SphereCollider.h"

//TEMP
#include "Runtime/Graphics/Material.h"
#include "Runtime/UI/UILib.h"


namespace Insight {

	namespace GameFramework {

		AActor::AActor(ActorId Id, ActorName ActorName)
			: m_Id(Id), m_NumComponents(0), m_DeltaMs(0.0f)
		{
			SceneNode::SetDisplayName(ActorName);
		}

		AActor::~AActor()
		{
		}

		bool AActor::LoadFromJson(const rapidjson::Value* jsonActor)
		{
			if (!m_CanBeFileParsed)
				return true;

			// Load Subobjects
			const rapidjson::Value& JsonSubobjects = (*jsonActor)["Subobjects"];

			for (UINT i = 0; i < JsonSubobjects.Size(); ++i) {

				if (JsonSubobjects[i].HasMember("SceneComponent")) {
					SceneComponent* ptr = AActor::CreateDefaultSubobject<SceneComponent>();
					ptr->LoadFromJson(JsonSubobjects[i]["SceneComponent"]);
					continue;
				}
				else if (JsonSubobjects[i].HasMember("StaticMesh")) {
					StaticMeshComponent* ptr = AActor::CreateDefaultSubobject<StaticMeshComponent>();
					ptr->LoadFromJson(JsonSubobjects[i]["StaticMesh"]);
					continue;
				}
				else if (JsonSubobjects[i].HasMember("CSharpScript")) {
					CSharpScriptComponent* ptr = AActor::CreateDefaultSubobject<CSharpScriptComponent>();
					ptr->LoadFromJson(JsonSubobjects[i]["CSharpScript"]);
					continue;
				}
				else if (JsonSubobjects[i].HasMember("SphereCollider")) {
					SphereColliderComponent* ptr = AActor::CreateDefaultSubobject<SphereColliderComponent>();
					//ptr->LoadFromJson(jsonSubobjects[i]["SphereCollider"]);
					continue;
				}
			}
			return true;
		}

		bool AActor::WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>* Writer)
		{
			if (!m_CanBeFileParsed)
				return true;

			Writer->StartObject(); // Start Write Actor
			{
				Writer->Key("Type");
				Writer->String("Actor");

				Writer->Key("DisplayName");
				Writer->String(StringHelper::WideToString(SceneNode::GetDisplayName()).c_str());

				Writer->Key("Subobjects");
				Writer->StartArray(); // Start Write SubObjects
				{
					for (size_t i = 0; i < m_NumComponents; ++i)
					{
						Writer->StartObject();
						m_Components[i]->WriteToJson(*Writer);
						Writer->EndObject();
					}
				}
				Writer->EndArray(); // End Write SubObjects
			}
			Writer->EndObject(); // End Write Actor
			return true;
		}

		void AActor::RenderSceneHeirarchy()
		{
			UI::NodeFlags TreeFlags = m_Children.empty() ? UI::TreeNode_Leaf : UI::TreeNode_OpenArrow | UI::TreeNode_OpenDoubleClick;
			TreeFlags |= UI::TreeNode_SpanAvailWidth;
			const bool IsExpanded = UI::TreeNodeEx(StringHelper::WideToString(SceneNode::GetDisplayName()).c_str(), TreeFlags);

			if (UI::IsItemClicked()) {
				IE_STRIP_FOR_GAME_DIST(Application::Get().GetEditorLayer().SetSelectedActor(this);)
			}

			if (IsExpanded) {

				SceneNode::RenderSceneHeirarchy();

				for (size_t i = 0; i < m_NumComponents; ++i) {
					m_Components[i]->RenderSceneHeirarchy();
				}

				UI::TreePopNode();
				UI::Spacing();
			}
		}

		static int currentIndex = 0;
		void AActor::OnImGuiRender()
		{
			if (UI::InputTextField("##ActorNameField", StringHelper::WideToString(m_DisplayName), UI::InputTextFieldFlags_EnterReturnsTrue)) {
				if (m_DisplayName == TEXT("")) {
					m_DisplayName = TEXT("MyActor");
				}
			}

			UI::NodeFlags TreeFlags = UI::TreeNode_Leaf;
			if (UI::TreeNodeEx("Actions", UI::TreeNode_OpenArrow)) {

				UI::TreeNodeEx("Delete Actor", TreeFlags);
				if (UI::IsItemClicked()) {

					// Set the Details panel to be blank
					Application::Get().GetEditorLayer().SetSelectedActor(nullptr);
					// remove the actor fom the world
					m_Parent->RemoveChild(this);
					// Pop the rest of the tree nodes for ImGui.
					// Thers no reason to leave this scope the actor has been deleted.
					UI::TreePopNode();
					UI::TreePopNode();
					return;
				}
				UI::TreePopNode();
				/*UI::TreeNodeEx("Remove All Components", TreeFlags);
				if (UI::IsItemClicked()) {
					RemoveAllSubobjects();
				}
				UI::TreePop();*/

				UI::TreePopNode();
			}

			UI::Spacing();

			// Add new component drop down
			{
				UI::NewLine();
				static constexpr char* availableComponents[] = { "", "Static Mesh Component", "C-Sharp Script Component" };
				if (UI::ComboBox("Add Component", currentIndex, availableComponents, _countof(availableComponents))) {
					switch (currentIndex) {
					case 0: break;
					case 1:
					{
						IE_LOG(Log, TEXT("Adding Static Mesh Component to \"%s\""), AActor::GetDisplayName());
						StaticMeshComponent* ptr = AActor::CreateDefaultSubobject<StaticMeshComponent>();
						ptr->SetMaterial(std::move(Material::CreateDefaultTexturedMaterial()));
						ptr->AttachMesh(TEXT("Models/Quad.obj"));

						break;
					}
					case 2:
					{
						IE_LOG(Log, TEXT("Adding C-Sharp Script Component to \"%s\""), AActor::GetDisplayName());
						CSharpScriptComponent* ptr = AActor::CreateDefaultSubobject<CSharpScriptComponent>();
						break;
					}
					default:
					{
						IE_LOG(Log, TEXT("Failed to determine component to add to actor \"%s\" with index of \"%i\""), AActor::GetDisplayName(), currentIndex);
						break;
					}
					}
				}

				UI::NewLine();
			}

			// Render each components details panels
			for (size_t i = 0; i < m_NumComponents; ++i)
			{
				UI::Spacing();
				m_Components[i]->OnImGuiRender();
			}
		}

		bool AActor::OnInit()
		{
			SceneNode::OnInit();
			return true;
		}

		bool AActor::OnPostInit()
		{
			SceneNode::OnPostInit();

			for (uint32_t i = 0; i < m_NumComponents; ++i)
			{
				m_Components[i]->OnPostInit();
			}
			return true;
		}

		void AActor::OnUpdate(const float DeltaMs)
		{
			SceneNode::OnUpdate(DeltaMs);
			m_DeltaMs = DeltaMs;

			for (size_t i = 0; i < m_NumComponents; ++i)
			{
				m_Components[i]->OnUpdate(DeltaMs);
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

		void AActor::EditorEndPlay()
		{
			SceneNode::EditorEndPlay();
			for (uint32_t i = 0; i < m_NumComponents; ++i) {
				m_Components[i]->EditorEndPlay();
			}
		}

		void AActor::Tick(const float DeltaMs)
		{
			SceneNode::Tick(DeltaMs);
			for (auto& comp : m_Components)
			{
				comp->Tick(DeltaMs);
			}
		}

		void AActor::Exit()
		{
			SceneNode::Exit();

		}

		void AActor::Destroy()
		{
			SceneNode::Destroy();

			for (uint32_t i = 0; i < m_NumComponents; i++) {
				m_Components[i]->OnDestroy();
				delete m_Components[i];
			}
			m_Components.clear();
		}

		void AActor::OnEvent(Event& e)
		{
			EventDispatcher Dispatcher(e);
			Dispatcher.Dispatch<PhysicsEvent>(IE_BIND_LOCAL_EVENT_FN(AActor::OnCollision));

			for (uint32_t i = 0; i < m_NumComponents; ++i)
			{
				m_Components[i]->OnEvent(e);
				if (e.Handled()) break;
			}
		}

		void AActor::RemoveSubobject(ActorComponent* component)
		{
			IE_ASSERT(std::find(m_Components.begin(), m_Components.end(), component) != m_Components.end(), "Could not find Component in Actor list while attempting to delete");

			auto iter = std::find(m_Components.begin(), m_Components.end(), component);
			(*iter)->OnDetach();
			(*iter)->OnDestroy();
			m_Components.erase(iter);
			m_NumComponents--;
		}

		void AActor::RemoveAllSubobjects()
		{
			for (uint32_t i = 0; i < m_NumComponents; ++i) {
				m_Components[i]->OnDestroy();
				delete m_Components[i];
			}
			m_Components.clear();
			m_NumComponents = 0;
		}

		bool AActor::OnCollision(PhysicsEvent& e)
		{
			IE_LOG(Log, TEXT("Physics Collision"));
			return false;
		}

	} // end namespace GameFramework
} // end namespace Insight
