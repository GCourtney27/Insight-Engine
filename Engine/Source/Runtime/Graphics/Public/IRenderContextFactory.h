#pragma once

#include <Runtime/Core.h>

namespace Insight
{
	namespace Graphics
	{
		class IRenderContext;

		class IDevice;
		class ISwapChain;
		class ICommandManager;

		class INSIGHT_API IRenderContextFactory
		{
		protected:

			virtual void CreateContext(IRenderContext** OutContext, std::shared_ptr<Window> InWindow) = 0;

			virtual void CreateDevice(IDevice** OutDevice) = 0;
			virtual void CreateSwapChain(ISwapChain** OutSwapChain, ICommandManager* InCommandManager) = 0;
			virtual void CreateCommandManager(ICommandManager** OutCommandManager, IDevice* InDevice) = 0;

		protected:
			IRenderContextFactory() {}
			virtual ~IRenderContextFactory() {}

		};
	
	}
}