#pragma once

#include <Runtime/Core.h>


namespace Insight
{
	enum EWindowMode;

	namespace Graphics
	{
		class IDevice;
		class ISwapChain;
		class ICommandManager;
		class IContextManager;

		enum class ERenderBackend
		{
			// TODO: Direct3D_11,
			Direct3D_12,
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
			FORCE_INLINE ISwapChain* GetSwapChain()				const { return m_pSwapChain; }
			FORCE_INLINE std::shared_ptr<Window> GetWindow()	const { return m_pWindow; }
			
			void SetWindow(std::shared_ptr<Window> pWindow);

		protected:
			IRenderContext()
				:  m_pSwapChain(NULL)
			{
			}

			FORCE_INLINE ISwapChain** GetSwapChainAddress() { return &m_pSwapChain; }


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
