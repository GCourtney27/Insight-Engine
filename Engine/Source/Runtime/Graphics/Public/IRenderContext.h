#pragma once

#include <Runtime/Core.h>


namespace Insight
{
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

			void OnFullScreenToggled(bool FullScreenEnabled);
			void OnNativeResolutionChanged(const FVector2& NewResolution);
			void EnableVSync(bool VsyncEnabled);

			// Debug Utilites
			// void DrawLine(FVector3 Start, FVector3 End, Color LineColor)
			// void DrawOnScreenText(FVector2 Location, Color TextColor)
			// void DrawRay(FVector3 Start, FVector3 Direction, Color LineColor, bool bIgnoreDepth)

			//
			// Getters/Setters
			//
			FORCE_INLINE IDevice** GetDevice()						{ return &m_pDevice;	}
			FORCE_INLINE ISwapChain** GetSwapChain()				{ return &m_pSwapChain; }
			FORCE_INLINE std::shared_ptr<Window> GetWindow() const	{ return m_pWindow;		}
			
			void SetWindow(std::shared_ptr<Window> pWindow);

		protected:
			IRenderContext()
				: m_pDevice(NULL)
				, m_pSwapChain(NULL)
			{
			}

			virtual void Initialize() = 0;
			virtual void UnInitialize();

		protected:

			std::shared_ptr<Window> m_pWindow;

			IDevice* m_pDevice;
			ISwapChain* m_pSwapChain;

			std::vector<ieVertexBuffer> m_VertexBuffers;
			std::vector<ieIndexBuffer> m_IndexBuffers;

		};
	}
} 
