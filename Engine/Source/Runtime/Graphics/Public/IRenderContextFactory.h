#pragma once

#include "EngineDefines.h"
#include <Runtime/Graphics/Public/GraphicsCore.h>

#include "Graphics/Public/IRenderContext.h"


namespace Insight
{
	namespace Graphics
	{
		class IRenderContext;

		class IDevice;
		class ISwapChain;
		class ICommandManager;
		class IContextManager;
		class IDescriptorAllocator;
		class IGeometryBufferManager;

		class INSIGHT_API IRenderContextFactory
		{
		protected:

			virtual void CreateContext(IRenderContext** OutContext, std::shared_ptr<Window> InWindow) = 0;

			FORCEINLINE void InitializeMainComponents();

			virtual void CreateDevice(IDevice** OutDevice) = 0;
			virtual void CreateSwapChain(ISwapChain** OutSwapChain, ICommandManager* InCommandManager, IDevice* InDevice) = 0;
			virtual void CreateCommandManager(ICommandManager** OutCommandManager, IDevice* InDevice) = 0;
			virtual void CreateContextManager(IContextManager** OutCommandContext) = 0;
			virtual void CreateGeometryManager(IGeometryBufferManager** OutGeometryManager) = 0;
			virtual void CreateConstantBufferManager(IConstantBufferManager** OutCBManager) = 0;
			virtual void CreateTextureManager(ITextureManager** OutTexManager) = 0;

		protected:
			IRenderContextFactory()
				: m_pTarget(NULL)
			{
			}
			virtual ~IRenderContextFactory() 
			{
				m_pTarget = NULL;
			}
			
			IRenderContext* m_pTarget;
		};
	
		//
		// IRenderContextFactory inline function implementations
		//

		FORCEINLINE void IRenderContextFactory::InitializeMainComponents()
		{
			CreateDevice(&g_pDevice);

			CreateCommandManager(&g_pCommandManager, g_pDevice);
			CreateContextManager(&g_pContextManager);
			CreateSwapChain(m_pTarget->GetSwapChainAddress(), g_pCommandManager, g_pDevice);
		}
	}
}