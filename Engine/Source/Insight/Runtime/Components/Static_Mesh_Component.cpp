#include <ie_pch.h>

#include "Insight/Runtime/AActor.h"
#include "Static_Mesh_Component.h"
#include "Insight/Systems/Model_Manager.h"
#include "imgui.h"

namespace Insight {



	StaticMeshComponent::StaticMeshComponent(StrongActorPtr owner)
		: ActorComponent("Static Mesh Component", owner)
	{
		//m_pModel = make_shared<Model>("../Assets/Models/Dandelion/Var1/Textured_Flower.obj");
		m_pModel = make_shared<Model>("../Assets/Models/nanosuit/nanosuit.obj");
		//m_pModel = make_shared<Model>("../Assets/Objects/Tiger/Tiger.obj");
		//m_pModel = make_shared<Model>("../Assets/Models/sponza/sponza.obj");
		ModelManager::Get().PushModel(m_pModel);
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

	void StaticMeshComponent::OnPreRender(XMMATRIX& parentMatrix)
	{
		m_pModel->PreRender(parentMatrix);
	}

	void StaticMeshComponent::OnRender()
	{
		// TODO: This is where we should request the draw call from the model manager by passing in the world transform
		//auto worldMat = ActorComponent::m_pOwner->GetTransform().GetWorldMatrix() * matrix;
		//ModelManager::Get().RequestDrawCall(m_pModel, )
		//m_pModel->PushInstanceWorldMatrix(parentMatrix);
		
		//m_pModel->Render();
		//m_pModel->Draw();
	}

	void StaticMeshComponent::OnUpdate(const float& deltaTime)
	{
		

	}

	void StaticMeshComponent::OnImGuiRender()
	{
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

	void StaticMeshComponent::OnAttach()
	{
	}

	void StaticMeshComponent::OnDetach()
	{
	}

}
