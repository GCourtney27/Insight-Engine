#pragma once

#include <Runtime/Core.h>

namespace Insight
{
	namespace Graphics
	{
		class IDevice;
		class ISwapChain;
		class ICommandManager;

		enum class ERenderBackend
		{
			Direct3D_12,
		};

		class INSIGHT_API IRenderContext
		{
		public:
			friend class IRenderContextFactory;
			friend class D3D12RenderContextFactory;

		public:
			virtual ~IRenderContext()
			{
				UnInitialize();
			}

			virtual void PreFrame() = 0;
			virtual void SubmitFrame() = 0;

			virtual void CreateTexture() = 0;
			virtual void CreateBuffer() = 0;
			virtual ieVertexBuffer& CreateVertexBuffer() = 0;
			virtual ieIndexBuffer& CreateIndexBuffer() = 0;

			virtual void BindVertexBuffer(const ieVertexBuffer& Vertexbuffer) = 0;
			virtual void BindIndexBuffer(const ieIndexBuffer& IndexBuffer) = 0;

			virtual void DrawMesh() = 0;

			void OnFullScreenToggled(bool FullScreenEnabled);
			void EnableVSync(bool VsyncEnabled);

			// Debug Utilites
			//void DrawLine(FVector3 Start, FVector3 End, Color LineColor)
			//void DrawOnScreenText(FVector2 Location, Color TextColor)
			//void DrawRay(FVector3 Start, FVector3 Direction, Color LineColor, bool bIgnoreDepth)

			inline IDevice** GetDevice()					{ return &m_pDevice; }
			inline ISwapChain** GetSwapChain()				{ return &m_pSwapChain; }
			inline ICommandManager** GetCommandManager()	{ return &m_pCommandManager; }
			
			void SetWindow(std::shared_ptr<Window> pWindow);
			inline std::shared_ptr<Window> GetWindow() const { return m_pWindow; }

		protected:
			IRenderContext()
				: m_pDevice(NULL)
				, m_pSwapChain(NULL)
				, m_pCommandManager(NULL)
			{
			}

			virtual void Initialize() = 0;
			virtual void UnInitialize();

		protected:
			std::shared_ptr<Window> m_pWindow;

			IDevice* m_pDevice;
			ISwapChain* m_pSwapChain;
			ICommandManager* m_pCommandManager;
			
			std::vector<ieVertexBuffer> m_VertexBuffers;
			std::vector<ieIndexBuffer> m_IndexBuffers;

		};
	}
} 
