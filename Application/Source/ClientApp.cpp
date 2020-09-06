#include "ClientApp.h"

#include "Insight/Runtime/AActor.h"

#include "Insight/Runtime/Components/Static_Mesh_Component.h"

namespace TankGame {


	/*bool Game::InitCoreApplication()
	{
		Super::InitCoreApplication();

		Insight::AActor* pTank = new Insight::AActor(0, "Tank");
		pTank->GetTransformRef().SetPosition({ 0.0f, 0.0f, 0.0f });
		pTank->GetTransformRef().SetRotation({ 0.0f, 0.0f, 0.0f });
		pTank->GetTransformRef().SetScale({ 1.0f, 1.0f, 0.0f });
		
		Insight::StaticMeshComponent* Component = reinterpret_cast<Insight::StaticMeshComponent*>(pTank->CreateDefaultSubobject<Insight::StaticMeshComponent>().get());
		Component->AttachMesh("Models/Tiger_NoTracks.obj");

		m_pGameLayer->GetScene()->AddActor(pTank);

		return true;
	}*/

	/*void Game::PostInit()
	{
		Super::PostInit();
	}

	void Game::Run()
	{
		Super::Run();
	}

	void Game::Shutdown()
	{
		Super::Shutdown();
	}*/

}

