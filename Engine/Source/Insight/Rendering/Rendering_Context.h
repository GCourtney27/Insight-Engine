#pragma once

/*
	Represents a base for a graphics context the application will use for rendering.
*/

namespace Insight {


	class RenderingContext
	{
	
	public:
		virtual ~RenderingContext() {}

		virtual bool Init() = 0;
		virtual bool PostInit() = 0;
		virtual void OnUpdate(const float& deltaTime) = 0;
		virtual void OnPreFrameRender() = 0;
		virtual void OnRender() = 0;
		virtual void OnMidFrameRender() = 0;
		virtual void ExecuteDraw() = 0;
		virtual void SwapBuffers() = 0;
		virtual void OnWindowResize() = 0;
		virtual void OnWindowFullScreen() = 0;

		inline uint8_t GetFrameBufferCount() const { return m_FrameBufferCount; }
		inline void SetVSyncEnabled(bool enabled) { m_VSyncEnabled = enabled; }
		inline void SetWindowWidthAndHeight(UINT width, UINT height, bool isMinimized) 
		{ 
			m_WindowWidth = width;
			m_WindowHeight = height;
			m_IsMinimized = isMinimized;
			m_AspectRatio = static_cast<float>(m_WindowWidth) / static_cast<float>(m_WindowHeight);
			OnWindowResize();
		}

	protected:
		RenderingContext(UINT windowWidth, UINT windowHeight, bool vSyncEabled)
			: m_WindowWidth(windowWidth), m_WindowHeight(windowHeight), m_VSyncEnabled(vSyncEabled) {}
	protected:
		static const uint8_t m_FrameBufferCount = 3u;
		UINT m_WindowWidth;
		UINT m_WindowHeight;

		float m_AspectRatio = 0.0f;

		bool m_VSyncEnabled = false;
		bool m_IsMinimized = false;
		bool m_FullScreenMode = false;
		bool m_WindowedMode = true;
		bool m_WindowVisible = true;
#define RETURN_IF_WINDOW_NOT_VISIBLE if (!m_WindowVisible){ return;}
		
		bool m_AllowTearing = true;

		enum RenderingAPI
		{
			D3D_11,
			OPENGL,

			D3D_12,
			VULKAN
		};
	};

}
