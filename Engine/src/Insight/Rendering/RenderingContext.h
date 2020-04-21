#pragma once


namespace Insight {

	class RenderingContext
	{
	
	public:
		virtual ~RenderingContext() {}

		virtual bool Init() = 0;
		virtual void OnUpdate() = 0;
		virtual void RenderFrame() = 0;
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
		static const uint8_t m_FrameBufferCount = 3;
		UINT m_WindowWidth;
		UINT m_WindowHeight;

		float m_AspectRatio = 0.0f;

		bool m_VSyncEnabled = false;
		bool m_IsMinimized = false;
		bool m_FullScreenMode = false;
		bool m_WindowedMode = true;
		bool m_WindowVisible = true;
		bool m_AllowTearing = true;

		enum RenderingAPI
		{
			// Dynamic Pipeline
			D3D_11,
			OPENGL,
			// Fixed Pipeline
			D3D_12,
			VULKAN
		};
	};

}
