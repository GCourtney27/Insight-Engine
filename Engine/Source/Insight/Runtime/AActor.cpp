#include <ie_pch.h>

#include "Insight/Runtime/Components/Actor_Component.h"
#include "Insight/Core/Application.h"
#include "AActor.h"
#include "imgui.h"

namespace Insight {


	AActor::AActor(ActorId id, ActorName actorName)
		: m_id(id)
	{
		SceneNode::SetDisplayName(actorName);

	}

	AActor::~AActor()
	{
	}

	void AActor::RenderSceneHeirarchy()
	{
		ImGuiTreeNodeFlags treeFlags = m_Children.empty() ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		const bool isExpanded = ImGui::TreeNodeEx(SceneNode::GetDisplayName(), treeFlags);

		if (ImGui::IsItemClicked()) {
			Application::Get().GetScene().SetSelectedActor(this);
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
			GetTransformRef().SetWorldMatrix(XMMatrixMultiply(parentMat, GetTransformRef().GetLocalMatrixRef()));
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

	}

	void AActor::Tick(const float& deltaMs)
	{
		SceneNode::Tick(deltaMs);

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

	void AActor::AddComponent(StrongActorComponentPtr pComponent)
	{
		IE_CORE_ASSERT(pComponent, "Attempting to add component to actor that is NULL");
		IE_CORE_ASSERT(std::find(m_Components.begin(), m_Components.end(), pComponent) == m_Components.end(), "Failed to add component to Actor");

		pComponent->OnAttach();
		m_Components.push_back(pComponent);
		m_NumComponents++;
	}

	void AActor::RemoveComponent(StrongActorComponentPtr component)
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

