#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/GraphicsCore.h"
#include "Runtime/Graphics/Public/IRenderContext.h"

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

		class INSIGHT_API IRenderContextFactory
		{
		protected:

			virtual void CreateContext(IRenderContext** OutContext, std::shared_ptr<Window> InWindow) = 0;

			FORCE_INLINE void InitializeMainComponents();

			virtual void CreateDevice(IDevice** OutDevice) = 0;
			virtual void CreateSwapChain(ISwapChain** OutSwapChain, ICommandManager* InCommandManager, IDevice* InDevice) = 0;
			virtual void CreateCommandManager(ICommandManager** OutCommandManager, IDevice* InDevice) = 0;
			virtual void CreateContextManager(IContextManager** OutCommandContext) = 0;


			template <typename DerivedType, typename BaseType, typename ... InitArgs>
			inline DerivedType* CreateRenderComponentObject(BaseType** ppBase, InitArgs ... args)
			{
				*ppBase = new DerivedType(args...);
				DerivedType* pDericedClass = DCast<DerivedType*>(*ppBase);
				IE_ASSERT(pDericedClass != NULL);

				return pDericedClass;
			}
			

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

		void IRenderContextFactory::InitializeMainComponents()
		{
			CreateDevice(&g_pDevice);

			CreateCommandManager(&g_pCommandManager, g_pDevice);
			CreateContextManager(&g_pContextManager);
			CreateSwapChain(m_pTarget->GetSwapChainAddress(), g_pCommandManager, g_pDevice);
		}
	}
}