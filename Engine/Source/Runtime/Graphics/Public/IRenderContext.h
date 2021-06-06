// Copyright Insight Interactive. All Rights Reserved.
#pragma once

#include "EngineDefines.h"
#include <Runtime/Graphics/Public/GraphicsCore.h>


namespace Insight
{
	enum EWindowMode;

	namespace Graphics
	{
		enum ERenderBackend
		{
			// TODO: RB_Direct3D11,
			RB_Direct3D12,
		};

		class INSIGHT_API IRenderContext
		{
			friend class IRenderContextFactory;
			friend class D3D12RenderContextFactory;
		public:
			virtual ~IRenderContext()
			{
				UnInitialize();
			}

			virtual void PreFrame() = 0;
			virtual void SubmitFrame() = 0;
			void Present();

			void OnWindowModeChanged(EWindowMode Mode);
			void OnNativeResolutionChanged(const FVector2& NewResolution);
			void EnableVSync(bool VsyncEnabled);

			//
			// Getters/Setters
			//
			FORCEINLINE ISwapChain* GetSwapChain()				const { return m_pSwapChain; }
			FORCEINLINE std::shared_ptr<Window> GetWindow()	const { return m_pWindow; }
			
			void SetWindow(std::shared_ptr<Window> pWindow);

		protected:
			IRenderContext()
				:  m_pSwapChain(NULL)
			{
			}

			FORCEINLINE ISwapChain** GetSwapChainAddress() { return &m_pSwapChain; }


			virtual void Initialize() = 0;
			virtual void UnInitialize();

		protected:

			std::shared_ptr<Window> m_pWindow;

			ISwapChain* m_pSwapChain;

			std::vector<ieVertexBuffer> m_VertexBuffers;
			std::vector<ieIndexBuffer> m_IndexBuffers;

		};
	}
} 
