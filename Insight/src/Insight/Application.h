#pragma once

#include "Core.h"

namespace Insight {


	class INSIGHT_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// TO be defined in client
	Application* CreateApplication();

}

