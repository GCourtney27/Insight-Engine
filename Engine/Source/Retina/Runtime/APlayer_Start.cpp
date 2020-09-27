#include <Engine_pch.h>

#include "APlayer_Start.h"

#include "Retina/Runtime/APlayer_Character.h"

namespace Retina {

	namespace Runtime {


		APlayerStart::APlayerStart(ActorId id, ActorName name)
			: AActor(id, name)
		{

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

		void APlayerStart::CalculateParent(XMMATRIX parentMat)
		{
			AActor::CalculateParent(parentMat);

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
			PlayerCharacter->GetTransformRef().SetPosition(GetTransform().GetPosition());
			//PlayerCharacter->GetTransformRef().SetDirection(GetTransform().GetLocalForward());
		}

	} // end namespace Runtime
} // end namespace Retina
