#include <ie_pch.h>

#include "Insight/Runtime/AActor.h"
#include "Static_Mesh_Component.h"
#include "Insight/Systems/Model_Manager.h"
#include "imgui.h"

namespace Insight {



	StaticMeshComponent::StaticMeshComponent(StrongActorPtr pOwner)
		: ActorComponent("Static Mesh Component", pOwner)
	{
		
	}

	StaticMeshComponent::~StaticMeshComponent()
	{
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

		ImGui::Text("Hello from Static mesh component");
	}

	void StaticMeshComponent::RenderSceneHeirarchy()
	{
		//if (ImGui::TreeNode(m_ComponentName))
		{
			m_pModel->RenderSceneHeirarchy();

			//ImGui::TreePop();
			//ImGui::Spacing();
		}
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
