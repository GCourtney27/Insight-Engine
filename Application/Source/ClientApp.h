#pragma once
#include <Retina.h>
#include "Retina/Runtime/Components/Static_Mesh_Component.h"
#include "Retina/Rendering/Material.h"
#include "Renderer/Platform/Windows/DirectX_12/Wrappers/rn_D3D12_Texture.h"
#include "Renderer/Platform/Windows/DirectX_12/Direct3D12_Context.h"

/*=====================================================================

	This class is your game. Include all game logic here it will get
	called and synced with the engine. If you would like to change the
	title of your game, Edit the macro 'Game'. Should you like to edit
	the macro, makk sure to change the name in all places of in the class.

 ======================================================================*/

#define Game Sandbox

namespace SandBoxApp {

	using Super = Retina::Application;

	class Game : public Retina::Application
	{
	public:
		Game()
		{
		}

		virtual ~Game()
		{
		}

		virtual bool InitializeCoreApplication() override
		{
			Super::InitializeCoreApplication();

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
			using namespace Retina::Runtime;
			using namespace Retina;

			// House Resting Terrain
			Retina::Material* pRestingTerrainMat = new Retina::Material({ 6, 7, 8, 9, 10 });
			pRestingTerrainMat->SetUVTilingOffset(9.0f, 9.0f);
			AActor* pARestingTerrain = new AActor(0, "HouseRestingTerrain");
			pARestingTerrain->GetTransformRef().SetPosition(ieVector3(-14.7f, -6.3f, 9.2f));
			pARestingTerrain->GetTransformRef().SetRotation(ieVector3(0.0f, 3.125f, 0.0f));
			pARestingTerrain->GetTransformRef().SetScale(ieVector3(10.786f, 10.786f, 10.786f));
			pARestingTerrain->OnInit();
			StaticMeshComponent* pStaticMesh = pARestingTerrain->CreateDefaultSubobject<StaticMeshComponent>();
			pStaticMesh->SetMaterial(pRestingTerrainMat);
			pStaticMesh->AttachMesh("Models/HouseRestingTerrain.obj");

			m_pGameLayer->GetScene()->AddActor(pARestingTerrain);


			// House
			AActor* pAHouse = new AActor(0, "House");
			pAHouse->GetTransformRef().SetPosition(ieVector3(-56.550f, 18.6f, -57.2f));
			pAHouse->GetTransformRef().SetScale(ieVector3(0.651f, 0.651f, 0.651f));
			pAHouse->GetTransformRef().SetRotation(ieVector3(0.0f, 1.6f, 0.0f));
			pAHouse->OnInit();
			// Outer House
			Retina::Material* pOuterHouseMat = new Retina::Material({ 16, 17, 18, 19, 20 });
			pOuterHouseMat->SetColorAddative(97.0f / 255.0f, 0.0f, 0.0f);
			pOuterHouseMat->SetUVTilingOffset(9.0f, 9.0f);
			StaticMeshComponent* pOuterHouse = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pOuterHouse->SetMaterial(pOuterHouseMat);
			pOuterHouse->AttachMesh("Objects/Norway/House/Outer_House.obj");
			// Inner House
			StaticMeshComponent* pInnerHouse = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pInnerHouse->SetMaterial(Material::CreateDefaultTexturedMaterial());
			pInnerHouse->AttachMesh("Objects/Norway/House/Inner_House.obj");
			// Floor
			StaticMeshComponent* pFloor = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pFloor->SetMaterial(Material::CreateDefaultTexturedMaterial());
			pFloor->AttachMesh("Objects/Norway/House/Floor.obj");
			// Roof
			Retina::Material* pRoofMat = new Retina::Material({ 26, 27, 28, 29, 30 });
			pRoofMat->SetUVTilingOffset(6.0f, 6.0f);
			StaticMeshComponent* pRoof = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pRoof->SetMaterial(pRoofMat);
			pRoof->AttachMesh("Objects/Norway/House/Roof_Metal.obj");
			// Door-Back
			StaticMeshComponent* pDoorBack = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pDoorBack->SetMaterial(Material::CreateDefaultTexturedMaterial());
			pDoorBack->AttachMesh("Objects/Norway/House/Door_Back.obj");
			// Door-Front
			StaticMeshComponent* pDoorFront = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pDoorFront->SetMaterial(Material::CreateDefaultTexturedMaterial());
			pDoorFront->AttachMesh("Objects/Norway/House/Door_Front.obj");
			// Window Frames
			Retina::Material* pWindowFramesMat = new Retina::Material({ 21, 22, 23, 24, 25 });
			pWindowFramesMat->SetUVTilingOffset(2.0f, 2.0f);
			StaticMeshComponent* pWindowFrames = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pWindowFrames->SetMaterial(pWindowFramesMat);
			pWindowFrames->AttachMesh("Objects/Norway/House/Window_Frames.obj");
			// Roof Trim-Left
			Retina::Material* pRoofTrimMat = new Retina::Material({ 21, 22, 23, 24, 25 });
			pRoofTrimMat->SetUVTilingOffset(7.0f, 7.0f);
			StaticMeshComponent* pRoofTrimLeft = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pRoofTrimLeft->SetMaterial(pRoofTrimMat);
			pRoofTrimLeft->AttachMesh("Objects/Norway/House/RoofTrim_Left.obj");
			// Roof Trim-Right
			StaticMeshComponent* pRoofTrimRight = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pRoofTrimRight->SetMaterial(pRoofTrimMat);
			pRoofTrimRight->AttachMesh("Objects/Norway/House/RoofTrim_Right.obj");
			// Entry Stairs
			StaticMeshComponent* pEntryStairs = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pEntryStairs->SetMaterial(Material::CreateDefaultTexturedMaterial());
			pEntryStairs->AttachMesh("Objects/Norway/House/Stairs_Entry.obj");
			// House Supports
			StaticMeshComponent* pHouseSupports = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pHouseSupports->SetMaterial(Material::CreateDefaultTexturedMaterial());
			pHouseSupports->AttachMesh("Objects/Norway/House/House_Supports.obj");
			pHouseSupports->SetPosition(ieVector3(-30.5f, -28.65f, 88.45f));
			pHouseSupports->SetRotation(ieVector3(0.0f, 1.55f, 0.0f));
			pHouseSupports->SetScale(ieVector3(40.0f, 40.0f, 40.0f));
			// Foliage Supports
			StaticMeshComponent* pFoliageSupports = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pFoliageSupports->SetMaterial(Material::CreateDefaultTexturedMaterial());
			pFoliageSupports->AttachMesh("Objects/Norway/House/Foliage_Supports.obj");
			// Patio
			Retina::Material* pPatioMat = new Retina::Material({ 11, 12, 13, 14, 15});
			StaticMeshComponent* pPatio = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pPatio->SetMaterial(pPatioMat);
			pPatio->AttachMesh("Objects/Norway/House/Patio.obj");
			pPatio->SetPosition(ieVector3(-47.85f, -36.15f, 108.9f));
			pPatio->SetRotation(ieVector3(0.0f, 1.55f, 0.0f));
			pPatio->SetScale(ieVector3(50.0f, 50.0f, 50.0f));

			m_pGameLayer->GetScene()->AddActor(pAHouse);

			// Stars
			AActor* pAStairs = new AActor(0, "Stairs");
			pAStairs->GetTransformRef().SetPosition(ieVector3(31.2f, -3.13f, 6.268f));
			pAStairs->GetTransformRef().SetScale(ieVector3(30.334f, 30.334f, 30.334f));
			pAStairs->GetTransformRef().SetRotation(ieVector3(0.0f, 3.158f, 0.0f));
			pAStairs->OnInit();
			// Outer House
			Retina::Material* pStairsMat = new Retina::Material({ 1, 2, 3, 4, 5 });
			StaticMeshComponent* pStairsMesh = pAStairs->CreateDefaultSubobject<StaticMeshComponent>();
			pStairsMesh->SetMaterial(pStairsMat);
			pStairsMesh->AttachMesh("Objects/Norway/Stairs.obj");

			m_pGameLayer->GetScene()->AddActor(pAStairs);


			// DEBUG
			AActor* pAFrontRocks = new AActor(0, "Front Rocks");
			/*pAFrontRocks->GetTransformRef().SetPosition(ieVector3(14.5f, 1.9f, -81.0f));
			pAFrontRocks->GetTransformRef().SetScale(ieVector3(0.063f, 0.063f, 0.063f));
			pAFrontRocks->GetTransformRef().SetRotation(ieVector3(0.0f, 3.158f, 0.0f));*/
			pAFrontRocks->GetTransformRef().SetPosition(ieVector3(49.5f, 38.9f, -16.0f));
			pAFrontRocks->GetTransformRef().SetScale(ieVector3(6.0f, 6.0f, 6.0f));
			pAFrontRocks->GetTransformRef().SetRotation(ieVector3(0.0f, 0.0f, 0.0f));
			pAFrontRocks->OnInit();

			Retina::Material* pFrontRocksMat = new Retina::Material({ 36, 37, 38, 39, 40 });
			StaticMeshComponent* pFrontRocks = pAFrontRocks->CreateDefaultSubobject<StaticMeshComponent>();
			pFrontRocks->SetMaterial(pFrontRocksMat);
			pFrontRocks->AttachMesh("Models/Sphere.obj");
			pFrontRocks->SetRotation(ieVector3(0.5f, 0.0f, 0.0f));

			m_pGameLayer->GetScene()->AddActor(pAFrontRocks);

			// Front Rocks
			/*AActor* pAFrontRocks = new AActor(0, "Front Rocks");
			pAFrontRocks->GetTransformRef().SetPosition(ieVector3(14.5f, 1.9f, -81.0f));
			pAFrontRocks->GetTransformRef().SetScale(ieVector3(0.063f, 0.063f, 0.063f));
			pAFrontRocks->GetTransformRef().SetRotation(ieVector3(0.0f, 3.158f, 0.0f));
			pAFrontRocks->OnInit();

			Retina::Material* pFrontRocksMat = new Retina::Material({ 36, 37, 38, 39, 40 });
			StaticMeshComponent* pFrontRocks = pAFrontRocks->CreateDefaultSubobject<StaticMeshComponent>();
			pFrontRocks->SetMaterial(pFrontRocksMat);
			pFrontRocks->AttachMesh("Objects/Norway/Opaque/CoastRock/CoastRock_LOD2.obj");
			pFrontRocks->SetRotation(ieVector3(0.5f, 0.0f, 0.0f));

			m_pGameLayer->GetScene()->AddActor(pAFrontRocks);*/

		}

	};

}

Retina::Application* Retina::CreateApplication()
{
	return new SandBoxApp::Game();
}