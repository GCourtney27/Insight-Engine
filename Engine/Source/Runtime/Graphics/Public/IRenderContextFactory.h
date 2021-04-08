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
				DerivedType* pNewValue = DCast<DerivedType*>(*ppBase);
				IE_ASSERT(pNewValue != NULL);

				return pNewValue;
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
			CreateDevice(m_pTarget->GetDeviceAddress());

			CreateCommandManager(&g_pCommandManager, *m_pTarget->GetDeviceAddress());
			CreateContextManager(&g_pContextManager);
			CreateSwapChain(m_pTarget->GetSwapChainAddress(), g_pCommandManager, *(m_pTarget->GetDeviceAddress()));
		}
	}
}