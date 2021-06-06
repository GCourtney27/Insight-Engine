#pragma once

#include <Insight.h>
#include <resource.h>

/*=====================================================================
	This class is your game. Include all gameplay logic here it will get
	called and synced with the engine.
 ======================================================================*/


namespace SandBoxApp {

	using Super = Insight::Engine;

	class Win32SandboxApp : public Insight::Engine
	{
	public:
		Win32SandboxApp()
		{
		}

		virtual ~Win32SandboxApp()
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
		virtual EErrorCode Run() override
		{
			return Super::Run();
		}

		// Shutdown the application and release all resources.
		virtual void Shutdown() override
		{
			Super::Shutdown();
		}

	private:

		void Norway()
		{
			using namespace Insight::GameFramework;
			using namespace Insight;

			// House Resting Terrain
			Insight::Material* pRestingTerrainMat = new Insight::Material({ 6, 7, 8, 9, 10 });
			pRestingTerrainMat->SetUVTilingOffset(9.0f, 9.0f);
			AActor* pARestingTerrain = new AActor(0, TEXT("HouseRestingTerrain"));
			SceneComponent* pSCHouseTerrain = pARestingTerrain->CreateDefaultSubobject<SceneComponent>();
			pSCHouseTerrain->SetPosition(-14.7f, -6.3f, 9.2f);
			pSCHouseTerrain->SetScale(10.786f, 10.786f, 10.786f);
			pSCHouseTerrain->SetRotation(0.0f, 3.125f, 0.0f);
			StaticMeshComponent* pStaticMesh = pARestingTerrain->CreateDefaultSubobject<StaticMeshComponent>();
			pStaticMesh->SetMaterial(pRestingTerrainMat);
			pStaticMesh->AttachMesh(TEXT("Models/HouseRestingTerrain.obj"));

			GetScene().AddActor(pARestingTerrain);


			// House
			AActor* pAHouse = new AActor(0, TEXT("House"));
			SceneComponent* pSCHouse = pAHouse->CreateDefaultSubobject<SceneComponent>();
			pSCHouse->SetPosition(-56.550f, 18.6f, -57.2f);
			pSCHouse->SetScale(0.651f, 0.651f, 0.651f);
			pSCHouse->SetRotation(0.0f, 1.6f, 0.0f);
			// Outer House
			Insight::Material* pOuterHouseMat = new Insight::Material({ 16, 17, 18, 19, 20 });
			pOuterHouseMat->SetColorAddative(97.0f / 255.0f, 0.0f, 0.0f);
			pOuterHouseMat->SetUVTilingOffset(9.0f, 9.0f);
			StaticMeshComponent* pOuterHouse = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pOuterHouse->SetMaterial(pOuterHouseMat);
			pOuterHouse->AttachMesh(TEXT("Objects/Norway/House/Outer_House.obj"));
			// Inner House
			StaticMeshComponent * pInnerHouse = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pInnerHouse->SetMaterial(Material::CreateDefaultTexturedMaterial());
			pInnerHouse->AttachMesh(TEXT("Objects/Norway/House/Inner_House.obj"));
			// Floor
			StaticMeshComponent * pFloor = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pFloor->SetMaterial(Material::CreateDefaultTexturedMaterial());
			pFloor->AttachMesh(TEXT("Objects/Norway/House/Floor.obj"));
			// Roof
			Insight::Material * pRoofMat = new Insight::Material({ 26, 27, 28, 29, 30 });
			pRoofMat->SetUVTilingOffset(6.0f, 6.0f);
			StaticMeshComponent * pRoof = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pRoof->SetMaterial(pRoofMat);
			pRoof->AttachMesh(TEXT("Objects/Norway/House/Roof_Metal.obj"));
			// Door-Back
			StaticMeshComponent* pDoorBack = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pDoorBack->SetMaterial(Material::CreateDefaultTexturedMaterial());
			pDoorBack->AttachMesh(TEXT("Objects/Norway/House/Door_Back.obj"));
			// Door-Front
			StaticMeshComponent * pDoorFront = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pDoorFront->SetMaterial(Material::CreateDefaultTexturedMaterial());
			pDoorFront->AttachMesh(TEXT("Objects/Norway/House/Door_Front.obj"));
			// Window Frames
			Insight::Material * pWindowFramesMat = new Insight::Material({ 21, 22, 23, 24, 25 });
			pWindowFramesMat->SetUVTilingOffset(2.0f, 2.0f);
			StaticMeshComponent * pWindowFrames = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pWindowFrames->SetMaterial(pWindowFramesMat);
			pWindowFrames->AttachMesh(TEXT("Objects/Norway/House/Window_Frames.obj"));
			// Roof Trim-Left
			Insight::Material * pRoofTrimMat = new Insight::Material({ 21, 22, 23, 24, 25 });
			pRoofTrimMat->SetUVTilingOffset(7.0f, 7.0f);
			StaticMeshComponent * pRoofTrimLeft = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pRoofTrimLeft->SetMaterial(pRoofTrimMat);
			pRoofTrimLeft->AttachMesh(TEXT("Objects/Norway/House/RoofTrim_Left.obj"));
			// Roof Trim-Right
			StaticMeshComponent * pRoofTrimRight = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pRoofTrimRight->SetMaterial(pRoofTrimMat);
			pRoofTrimRight->AttachMesh(TEXT("Objects/Norway/House/RoofTrim_Right.obj"));
			// Entry Stairs
			StaticMeshComponent * pEntryStairs = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pEntryStairs->SetMaterial(Material::CreateDefaultTexturedMaterial());
			pEntryStairs->AttachMesh(TEXT("Objects/Norway/House/Stairs_Entry.obj"));
			// House Supports
			StaticMeshComponent * pHouseSupports = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pHouseSupports->SetMaterial(Material::CreateDefaultTexturedMaterial());
			pHouseSupports->AttachMesh(TEXT("Objects/Norway/House/House_Supports.obj"));
			pHouseSupports->SetPosition(FVector3(-30.5f, -28.65f, 88.45f));
			pHouseSupports->SetRotation(FVector3(0.0f, 1.55f, 0.0f));
			pHouseSupports->SetScale(FVector3(40.0f, 40.0f, 40.0f));
			// Foliage Supports
			StaticMeshComponent * pFoliageSupports = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pFoliageSupports->SetMaterial(Material::CreateDefaultTexturedMaterial());
			pFoliageSupports->AttachMesh(TEXT("Objects/Norway/House/Foliage_Supports.obj"));
			// Patio
			Insight::Material * pPatioMat = new Insight::Material({ 11, 12, 13, 14, 15 });
			StaticMeshComponent * pPatio = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pPatio->SetMaterial(pPatioMat);
			pPatio->AttachMesh(TEXT("Objects/Norway/House/Patio.obj"));
			pPatio->SetPosition(FVector3(-47.85f, -36.15f, 108.9f));
			pPatio->SetRotation(FVector3(0.0f, 1.55f, 0.0f));
			pPatio->SetScale(FVector3(50.0f, 50.0f, 50.0f));

			GetScene().AddActor(pAHouse);

			// Stars
			AActor* pAStairs = new AActor(0, TEXT("Stairs"));
			SceneComponent* pSCStairs = pAStairs->CreateDefaultSubobject<SceneComponent>();
			pSCStairs->SetPosition(31.2f, -3.13f, 6.268f);
			pSCStairs->SetScale(30.334f, 30.334f, 30.334f);
			pSCStairs->SetRotation(0.0f, 3.158f, 0.0f);
			// Outer House
			Insight::Material* pStairsMat = new Insight::Material({ 1, 2, 3, 4, 5 });
			StaticMeshComponent* pStairsMesh = pAStairs->CreateDefaultSubobject<StaticMeshComponent>();
			pStairsMesh->SetMaterial(pStairsMat);
			pStairsMesh->AttachMesh(TEXT("Objects/Norway/Stairs.obj"));

			GetScene().AddActor(pAStairs);


			// DEBUG
			AActor* pASphere = new AActor(0, TEXT("Rusted Orb"));
			/*pAFrontRocks->GetTransformRef().SetPosition(FVector3(14.5f, 1.9f, -81.0f));
			pAFrontRocks->GetTransformRef().SetScale(FVector3(0.063f, 0.063f, 0.063f));
			pAFrontRocks->GetTransformRef().SetRotation(FVector3(0.0f, 3.158f, 0.0f));*/
			SceneComponent* pSCSphere = pASphere->CreateDefaultSubobject<SceneComponent>();
			pSCSphere->SetPosition(49.5f, 38.9f, -16.0f);
			pSCSphere->SetScale(6.0f, 6.0f, 6.0f);
			pSCSphere->SetRotation(0.0f, 0.0f, 0.0f);


			Insight::Material* pFrontRocksMat = new Insight::Material({ 36, 37, 38, 39, 40 });
			StaticMeshComponent* pFrontRocks = pASphere->CreateDefaultSubobject<StaticMeshComponent>();
			pFrontRocks->SetMaterial(pFrontRocksMat);
			//pFrontRocks->AttachMesh("Models/1911a1.fbx");
			pFrontRocks->AttachMesh(TEXT("Models/Sphere.obj"));
			pFrontRocks->SetRotation(FVector3(1.5f, 0.0f, 0.0f));

			GetScene().AddActor(pASphere);

			// Front Rocks
			/*AActor* pAFrontRocks = new AActor(0, "Front Rocks");
			pAFrontRocks->GetTransformRef().SetPosition(FVector3(14.5f, 1.9f, -81.0f));
			pAFrontRocks->GetTransformRef().SetScale(FVector3(0.063f, 0.063f, 0.063f));
			pAFrontRocks->GetTransformRef().SetRotation(FVector3(0.0f, 3.158f, 0.0f));

			Insight::Material* pFrontRocksMat = new Insight::Material({ 36, 37, 38, 39, 40 });
			StaticMeshComponent* pFrontRocks = pAFrontRocks->CreateDefaultSubobject<StaticMeshComponent>();
			pFrontRocks->SetMaterial(pFrontRocksMat);
			pFrontRocks->AttachMesh("Objects/Norway/Opaque/CoastRock/CoastRock_LOD2.obj");
			pFrontRocks->SetRotation(FVector3(0.5f, 0.0f, 0.0f));

			GetScene().AddActor(pAFrontRocks);*/
		}

		void FillScene()
		{
			using namespace Insight::GameFramework;
			using namespace Insight;
			
			//Norway();
			//return;

			// Wooden Floor
			Insight::Material* pWood = new Insight::Material({ 1, 2, 3, 4, 5 });
			pWood->SetUVTilingOffset(9.0f, 9.0f);
			AActor* pAWoodenFloor = new AActor(0, TEXT("Wooden Floor"));
			SceneComponent* pSCWoodenFloor = pAWoodenFloor->CreateDefaultSubobject<SceneComponent>();
			pSCWoodenFloor->SetScale(1000.0f);
			pSCWoodenFloor->SetPosition(0.0f, -20.0f, 0.0f);
			StaticMeshComponent* pSMQuad = pAWoodenFloor->CreateDefaultSubobject<StaticMeshComponent>();
			pSMQuad->SetMaterial(pWood);
			pSMQuad->AttachMesh(TEXT("Models/Quad.fbx"));
			GetScene().AddActor(pAWoodenFloor);

			// Rusted Ball
			Insight::Material* pRustedIron = new Insight::Material({ 6, 7, 8, 9, 10 });
			AActor* pARustedBall = new AActor(0, TEXT("Rusted Ball"));
			SceneComponent* pSCDemoBall = pARustedBall->CreateDefaultSubobject<SceneComponent>();
			pSCDemoBall->SetScale(20.0f);
			pSCDemoBall->SetPosition(10.0f, 40.0f, 0.0f);
			StaticMeshComponent* pSMCube = pARustedBall->CreateDefaultSubobject<StaticMeshComponent>();
			pSMCube->SetMaterial(pRustedIron);
			pSMCube->AttachMesh(TEXT("Models/Sphere.fbx"));
			GetScene().AddActor(pARustedBall);

			// Bamboo Ball
			AActor* pABambooBall = new AActor(0, TEXT("Bamboo Ball"));
			SceneComponent* pSCBall = pABambooBall->CreateDefaultSubobject<SceneComponent>();
			pSCBall->SetScale(20.0f);
			pSCBall->SetPosition(-50.0f, 40.0f, 0.0f);
			Insight::Material* pBamboo = new Insight::Material({ 11, 12, 13, 14, 15 });
			StaticMeshComponent* pSMBall = pABambooBall->CreateDefaultSubobject<StaticMeshComponent>();
			pSMBall->SetMaterial(pBamboo);
			pSMBall->AttachMesh(TEXT("Models/Sphere.fbx"));
			GetScene().AddActor(pABambooBall);
			
			// Wall
			AActor* pACube = new AActor(0, TEXT("Wall"));
			SceneComponent* pSCPistol = pACube->CreateDefaultSubobject<SceneComponent>();
			pSCPistol->SetScale(5.0f);
			pSCPistol->SetPosition(-300.0f, 60.0f, 0.0f);
			pSCPistol->SetScale(5.0f, 120.0f, 200.0f);
			Insight::Material* pCheckeredMat = new Insight::Material({ 16 });
			pCheckeredMat->SetUVTilingOffset(5.0f, 5.0f);
			StaticMeshComponent* pSMCube2 = pACube->CreateDefaultSubobject<StaticMeshComponent>();
			pSMCube2->SetMaterial(pCheckeredMat);
			pSMCube2->AttachMesh(TEXT("Models/Cube.fbx"));
			GetScene().AddActor(pACube);

		}

	};

}

