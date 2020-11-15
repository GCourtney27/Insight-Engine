#pragma once

#include <Insight/Core.h>

#include "Insight/Runtime/AActor.h"

namespace Insight {

	namespace Runtime {

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