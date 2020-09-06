#pragma once
#include <Insight.h>
#include "Insight/Runtime/Components/Static_Mesh_Component.h"

/*=====================================================================

	This class is your game. Include all game logic here it will get
	called and synced with the engine. If you would like to change the
	title of your game, Edit the macro 'Game'. Should you like to edit
	the macro, makk sure to change the name in all places of in the class.

 ======================================================================*/

#define Game Sandbox

namespace TankGame {

	using Super = Insight::Application;

	class Game : public Insight::Application
	{
	public:
		Game()
		{
			//PushLayer(new ExampleLayer());
		}

		virtual ~Game()
		{

		}

		virtual bool InitCoreApplication() override
		{
			Super::InitCoreApplication();

			Insight::AActor* pTank = new Insight::AActor(0, "Tank");
			pTank->GetTransformRef().SetPosition({ 0.0f, 0.0f, 0.0f });
			pTank->GetTransformRef().SetRotation({ 0.0f, 0.0f, 0.0f });
			pTank->GetTransformRef().SetScale({ 1.0f, 1.0f, 0.0f });
			pTank->OnInit();

			m_pGameLayer->GetScene()->AddActor(pTank);

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
	};

}

Insight::Application* Insight::CreateApplication()
{
	return new TankGame::Game();
}