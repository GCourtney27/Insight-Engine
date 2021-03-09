#pragma once

#include <Insight.h>


namespace SandBoxApp {

	using Super = Insight::Application;

	class UWPSandboxApp : public Insight::Application
	{
	public:
		UWPSandboxApp()
		{
		}

		virtual ~UWPSandboxApp()
		{
		}

		virtual void Initialize() override
		{
			Super::Initialize();

			FillScene();

			Super::PostInit();
		}

		// Called when the main portion of the applicaiton has been initialized.
		virtual void PostInit() override
		{
			Super::PostInit();
		}

		// Main loop of the application. This is the main entry point for every frame.
		virtual Application::EErrorCode Run() override
		{
			return Super::Run();
		}

		// Shutdown the application and release all resources.
		virtual void Shutdown() override
		{
			Super::Shutdown();
		}

	private:

		void FillScene()
		{
			using namespace Insight::GameFramework;
			using namespace Insight;


			// Wooden Floor
			Insight::Material* pWood = new Insight::Material({ 1, 2, 3, 4, 5 });
			pWood->SetUVTilingOffset(9.0f, 9.0f);
			AActor* pAWoodenFloor = new AActor(0, TEXT("Wooden Floor"));
			SceneComponent* pSCWoodenFloor = pAWoodenFloor->CreateDefaultSubobject<SceneComponent>();
			pSCWoodenFloor->SetScale(1000.0f);
			pSCWoodenFloor->SetPosition(0.0f, -20.0f, 0.0f);
			StaticMeshComponent* pSMQuad = pAWoodenFloor->CreateDefaultSubobject<StaticMeshComponent>();
			pSMQuad->SetMaterial(pWood);
			//pSMQuad->AttachMesh("Models/M1911_01.FBX");
			pSMQuad->AttachMesh(TEXT("Models/Quad.fbx"));
			GetScene().AddActor(pAWoodenFloor);

			// Rusted Ball
			AActor* pARustedBall = new AActor(0, TEXT("Rusted Ball"));
			SceneComponent* pSCDemoBall = pARustedBall->CreateDefaultSubobject<SceneComponent>();
			pSCDemoBall->SetScale(20.0f);
			pSCDemoBall->SetPosition(10.0f, 40.0f, 0.0f);
			Insight::Material* pRustedIron = new Insight::Material({ 6, 7, 8, 9, 10 });
			StaticMeshComponent* pSMCube = pARustedBall->CreateDefaultSubobject<StaticMeshComponent>();
			pSMCube->SetMaterial(pRustedIron);
			pSMCube->AttachMesh(TEXT("Models/Sphere.fbx"));
			GetScene().AddActor(pARustedBall);

			// Bamboo Cube
			AActor* pABambooBall = new AActor(0, TEXT("Bamboo Cube"));
			SceneComponent* pSCBall = pABambooBall->CreateDefaultSubobject<SceneComponent>();
			pSCBall->SetScale(20.0f);
			pSCBall->SetPosition(-50.0f, 40.0f, 0.0f);
			Insight::Material* pBamboo = new Insight::Material({ 11, 12, 13, 14, 15 });
			StaticMeshComponent* pSMBall = pABambooBall->CreateDefaultSubobject<StaticMeshComponent>();
			pSMBall->SetMaterial(pBamboo);
			pSMBall->AttachMesh(TEXT("Models/Cube.fbx"));
			GetScene().AddActor(pABambooBall);

		}

	};

}
