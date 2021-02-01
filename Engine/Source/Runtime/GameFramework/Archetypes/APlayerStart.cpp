#include <Engine_pch.h>

#include "APlayerStart.h"

#include "Runtime/GameFramework/Archetypes/APlayerCharacter.h"

namespace Insight {

	namespace GameFramework {


		APlayerStart::APlayerStart(ActorId id, ActorName name)
			: AActor(id, name)
		{
			m_pSceneComponent = CreateDefaultSubobject<SceneComponent>();
		}

		APlayerStart::~APlayerStart()
		{
		}

		bool APlayerStart::OnInit()
		{
			AActor::OnInit();

			return true;
		}

		bool APlayerStart::OnPostInit()
		{
			AActor::OnPostInit();

			return true;
		}

		void APlayerStart::OnUpdate(const float DeltaMs)
		{
			AActor::OnUpdate(DeltaMs);

		}

		void APlayerStart::OnRender()
		{
			AActor::OnRender();

		}

		void APlayerStart::BeginPlay()
		{
		}

		void APlayerStart::Tick(const float DeltaMs)
		{
		}

		void APlayerStart::RenderSceneHeirarchy()
		{
			AActor::RenderSceneHeirarchy();

		}

		void APlayerStart::OnImGuiRender()
		{
			AActor::OnImGuiRender();

		}

		void APlayerStart::SpawnPlayer(APlayerCharacter* PlayerCharacter)
		{
			//PlayerCharacter->SetPosition(m_pSceneComponent->GetTransform().GetPositionFloat3());
			//PlayerCharacter->GetTransformRef().SetDirection(GetTransform().GetLocalForward());
		}

	} // end namespace GameFramework
} // end namespace Insight
