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
			pARestingTerrain->GetTransformRef().SetRotation(ieVector3(0.0f, 3.125f, 0.0f));
			pARestingTerrain->GetTransformRef().SetScale(ieVector3(9.0f, 9.0f, 9.0f));
			pARestingTerrain->OnInit();
			StaticMeshComponent* pStaticMesh = pARestingTerrain->CreateDefaultSubobject<StaticMeshComponent>();
			pStaticMesh->SetMaterial(std::move(Material::CreateDefaultTexturedMaterial()));
			pStaticMesh->AttachMesh("Models/HouseRestingTerrain.obj");

			m_pGameLayer->GetScene()->AddActor(pARestingTerrain);


			// House
			AActor* pAHouse = new AActor(0, "House");
			pAHouse->GetTransformRef().SetPosition(ieVector3(-56.550f, 18.6f, -57.2f));
			pAHouse->GetTransformRef().SetScale(ieVector3(0.651f, 0.651f, 0.651f));
			pAHouse->GetTransformRef().SetRotation(ieVector3(0.0f, 1.6f, 0.0f));
			pAHouse->OnInit();
			// Outer House
			StaticMeshComponent* pOuterHouse = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pOuterHouse->SetMaterial(std::move(Material::CreateDefaultTexturedMaterial()));
			pOuterHouse->AttachMesh("Objects/Norway/House/Outer_House.obj");
			// Inner House
			StaticMeshComponent* pInnerHouse = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pInnerHouse->SetMaterial(std::move(Material::CreateDefaultTexturedMaterial()));
			pInnerHouse->AttachMesh("Objects/Norway/House/Inner_House.obj");
			// Floor
			StaticMeshComponent* pFloor = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pFloor->SetMaterial(std::move(Material::CreateDefaultTexturedMaterial()));
			pFloor->AttachMesh("Objects/Norway/House/Floor.obj");
			// Roof
			StaticMeshComponent* pRoof = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pRoof->SetMaterial(std::move(Material::CreateDefaultTexturedMaterial()));
			pRoof->AttachMesh("Objects/Norway/House/Roof_Metal.obj");
			// Door-Back
			StaticMeshComponent* pDoorBack = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pDoorBack->SetMaterial(std::move(Material::CreateDefaultTexturedMaterial()));
			pDoorBack->AttachMesh("Objects/Norway/House/Door_Back.obj");
			// Door-Front
			StaticMeshComponent* pDoorFront = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pDoorFront->SetMaterial(std::move(Material::CreateDefaultTexturedMaterial()));
			pDoorFront->AttachMesh("Objects/Norway/House/Door_Front.obj");
			// Window Frames
			StaticMeshComponent* pWindowFrames = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pWindowFrames->SetMaterial(std::move(Material::CreateDefaultTexturedMaterial()));
			pWindowFrames->AttachMesh("Objects/Norway/House/Window_Frames.obj");
			// Roof Trim-Left
			StaticMeshComponent* pRoofTrimLeft = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pRoofTrimLeft->SetMaterial(std::move(Material::CreateDefaultTexturedMaterial()));
			pRoofTrimLeft->AttachMesh("Objects/Norway/House/RoofTrim_Left.obj");
			// Roof Trim-Right
			StaticMeshComponent* pRoofTrimRight = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pRoofTrimRight->SetMaterial(std::move(Material::CreateDefaultTexturedMaterial()));
			pRoofTrimRight->AttachMesh("Objects/Norway/House/RoofTrim_Right.obj");
			// Entry Stairs
			StaticMeshComponent* pEntryStairs = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pEntryStairs->SetMaterial(std::move(Material::CreateDefaultTexturedMaterial()));
			pEntryStairs->AttachMesh("Objects/Norway/House/Stairs_Entry.obj");
			// House Supports
			StaticMeshComponent* pHouseSupports = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pHouseSupports->SetMaterial(std::move(Material::CreateDefaultTexturedMaterial()));
			pHouseSupports->AttachMesh("Objects/Norway/House/House_Supports.obj");
			pHouseSupports->SetPosition(ieVector3(-30.5f, -28.65f, 88.45f));
			pHouseSupports->SetRotation(ieVector3(0.0f, 1.55f, 0.0f));
			pHouseSupports->SetScale(ieVector3(40.0f, 40.0f, 40.0f));
			// Foliage Supports
			StaticMeshComponent* pFoliageSupports = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pFoliageSupports->SetMaterial(std::move(Material::CreateDefaultTexturedMaterial()));
			pFoliageSupports->AttachMesh("Objects/Norway/House/Foliage_Supports.obj");
			// Patio
			StaticMeshComponent* pPatio = pAHouse->CreateDefaultSubobject<StaticMeshComponent>();
			pPatio->SetMaterial(std::move(Material::CreateDefaultTexturedMaterial()));
			pPatio->AttachMesh("Objects/Norway/House/Patio.obj");
			pPatio->SetPosition(ieVector3(-47.85f, -36.15f, 108.9f));
			pPatio->SetRotation(ieVector3(0.0f, 1.55f, 0.0f));
			pPatio->SetScale(ieVector3(50.0f, 50.0f, 50.0f));


			m_pGameLayer->GetScene()->AddActor(pAHouse);

		}

	};

}

Insight::Application* Insight::CreateApplication()
{
	return new SandBoxApp::Game();
}