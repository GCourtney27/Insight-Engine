#pragma once


namespace Insight {

	class RenderingContext
	{
	
	public:
		virtual ~RenderingContext() {}

		virtual bool Init() = 0;
		virtual void RenderFrame() = 0;
		virtual void SwapBuffers() = 0;
		virtual void OnWindowResize() = 0;

		inline uint8_t GetFrameBufferCount() const { return m_FrameBufferCount; }
		inline void SetVSyncEnabled(bool enabled) { m_VSyncEnabled = enabled; }

	protected:
		RenderingContext(uint32_t windowWidth, uint32_t windowHeight, bool vSyncEabled)
			: m_WindowWidth(windowWidth), m_WindowHeight(windowHeight), m_VSyncEnabled(vSyncEabled) {}
	protected:
		static const uint8_t m_FrameBufferCount = 3;
		uint32_t m_WindowWidth;
		uint32_t m_WindowHeight;
		bool m_VSyncEnabled = false;
		enum RenderingAPI
		{
			D3D_11,
			D3D_12
		};
	};

}
