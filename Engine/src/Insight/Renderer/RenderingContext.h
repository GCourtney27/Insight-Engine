#pragma once


namespace Insight {

	class RenderingContext
	{
	public:
		virtual void Init() = 0;
		virtual void RenderFrame() = 0;
		virtual void SwapBuffers() = 0;
	protected:
		RenderingContext(uint32_t windowWidth, uint32_t windowHeight)
			: m_WindowWidth(windowWidth), m_WindowHeight(windowHeight) {}
		virtual ~RenderingContext() {}
	protected:
		static const uint8_t m_FrameBufferCount = 3;
		uint32_t m_WindowWidth;
		uint32_t m_WindowHeight;
	};

}
