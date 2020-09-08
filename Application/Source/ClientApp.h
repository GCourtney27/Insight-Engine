#pragma once
#include <Insight.h>
#include "Insight/Runtime/Components/Static_Mesh_Component.h"
#include "Insight/Rendering/Material.h"

/*=====================================================================

	This class is your game. Include all game logic here it will get
	called and synced with the engine. If you would like to change the
	title of your game, Edit the macro 'Game'. Should you like to edit
	the macro, makk sure to change the name in all places of in the class.

 ======================================================================*/

#define Game Sandbox

namespace SandBoxApp {

	using Super = Insight::Application;

	class Game : public Insight::Application
	{
	public:
		Game()
		{
		}

		virtual ~Game()
		{
		}

		virtual bool InitCoreApplication() override
		{
			Super::InitCoreApplication();

			FillScene();

			return true;
		}
		 
		// Called when the main portion of the applicaiton has been initialized.
		virtual void PostInit() override
		{
			Super::PostInit();
		}

		// Main loop of the application. This is the main entry point for every frame.
		virtual void Run() override
		{
			Super::Run();
		}

		// Shutdown the application and release all resources.
		virtual void Shutdown() override
		{
			Super::Shutdown();
		}

	private:
		void FillScene()
		{
			using namespace Insight::Runtime;
			using namespace Insight;

			// House Resting Terrain
			AActor* pARestingTerrain = new AActor(0, "HouseRestingTerrain");
			pARestingTerrain->GetTransformRef().SetPosition(ieVector3(0.0f, 0.0f, 0.0f));
			pARestingTerrain->GetTransformRef().SetRotation(ieVector3(0.0f, 0.0f, 0.0f));
			pARestingTerrain->GetTransformRef().SetScale(ieVector3(1.0f, 1.0f, 1.0f));
			pARestingTerrain->OnInit();
			StaticMeshComponent* pStaticMesh = pARestingTerrain->CreateDefaultSubobject<StaticMeshComponent>();
			pStaticMesh->SetMaterial(std::move(Material::CreateDefaultTexturedMaterial()));
			pStaticMesh->AttachMesh("Models/HouseRestingTerrain.obj");

			m_pGameLayer->GetScene()->AddActor(pARestingTerrain);



		}

	};

}

Insight::Application* Insight::CreateApplication()
{
	return new SandBoxApp::Game();
}