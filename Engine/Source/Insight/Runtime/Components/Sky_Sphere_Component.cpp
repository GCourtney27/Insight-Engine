#include <ie_pch.h>

#include "Insight/Runtime/AActor.h"
#include "Insight/Rendering/Texture.h"
#include "Insight/Systems/File_System.h"
#include "Insight/Rendering/Geometry/Model.h"
#include "Insight/Runtime/Components/Static_Mesh_Component.h"

#include "Sky_Sphere_Component.h"


namespace Insight {



	SkySphereComponent::SkySphereComponent(StrongActorPtr pOwner)
		: ActorComponent("Static Mesh Component", pOwner)
	{
		pOwner->SetRenderPass(RenderPass::RenderPass_Sky);
	}

	SkySphereComponent::~SkySphereComponent()
	{
	}

	void SkySphereComponent::OnInit()
	{
	}

	void SkySphereComponent::OnPostInit()
	{
	}

	void SkySphereComponent::OnDestroy()
	{
	}

	void SkySphereComponent::OnPreRender(const DirectX::XMMATRIX& matrix)
	{
	}

	void SkySphereComponent::OnRender()
	{
	}

	void SkySphereComponent::OnUpdate(const float& deltaTime)
	{
	}

	void SkySphereComponent::OnChanged()
	{
	}

	void SkySphereComponent::OnImGuiRender()
	{
	}

	void SkySphereComponent::RenderSceneHeirarchy()
	{
	}

	void SkySphereComponent::OnAttach()
	{
		//StrongActorComponentPtr comp = m_pOwner->CreateDefaultSubobject<StaticMeshComponent>();
		//reinterpret_cast<StaticMeshComponent*>(comp.get())->AttachMesh(FileSystem::Get().GetRelativeAssetDirectoryPath("Models/Geometry/Sphere.obj"));

	}

	void SkySphereComponent::OnDetach()
	{
	}

}