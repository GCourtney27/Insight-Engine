#pragma once

#include <Runtime/Core.h>

#include "Runtime/GameFramework/AActor.h"

namespace Insight {

	namespace GameFramework {

		class InputComponent;

		class INSIGHT_API APlayerController : public AActor
		{
		public:
			APlayerController();
			~APlayerController();


		private:
			std::unordered_map<const char*, InputComponent> m_InputComponents;
		};
	}
}