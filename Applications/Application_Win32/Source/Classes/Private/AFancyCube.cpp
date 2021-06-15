#include "Classes/Public/AFancyCube.h"

#include "Core/Public/ieObject/Components/ieStaticMeshComponent.h"

AFancyActor::AFancyActor()
	: ieActor(GetWorld(), "Fancy Cube Actor")
{
	ieStaticMeshComponent* pMesh = CreateDefaultSubObject<ieStaticMeshComponent>(TEXT("Static mesh component"));
	pMesh->GetTransform().SetParent(&m_Transform);
}

AFancyActor::~AFancyActor()
{
}
