#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/IRenderContextFactory.h"

namespace Insight
{
	namespace Graphics
	{
		class IRenderContext;
		class IDevice;
		class ISwapChain;

		namespace DX12
		{
			class INSIGHT_API D3D12RenderContextFactory final : public IRenderContextFactory
			{
			public:
				D3D12RenderContextFactory();
				virtual ~D3D12RenderContextFactory();
				
				virtual void CreateContext(IRenderContext** OutContext, std::shared_ptr<Window> InWindow) override;

			protected:
				virtual void CreateDevice(IDevice** OutDevice) override;
				virtual void CreateSwapChain(ISwapChain** OutSwapChain, ICommandManager* InCommandManager) override;
				virtual void CreateCommandManager(ICommandManager** OutCommandManager, IDevice* InDevice) override;

				void CreateDXGIFactory();

				IDXGIFactory6* GetDXGIFactory() const { return m_pDXGIFactory; }

				IRenderContext* m_pTarget;

				// DirectX
				//
				IDXGIFactory6* m_pDXGIFactory;

			};

		}
	}
}