#pragma once
#include <Insight/Core.h>

#include "Insight/Core/Layer/Layer.h"

#include "Insight/Systems/Frame_Timer.h"
#include "Renderer/Renderer.h"

namespace Insight {

	class INSIGHT_API PerfOverlay : public Layer
	{
	public:
		PerfOverlay() {}
		~PerfOverlay() {}

		virtual void OnAttach() override {}
		virtual void OnDetach() override {}
		virtual void OnImGuiRender() override{}

		virtual void OnUpdate(const float DeltaMs) override
		{
			m_FrameTimer.Tick();
			char OutputBuffer[256];
			sprintf_s(OutputBuffer, "FPS: %f\nFrame Time: %fms", GetFPS(), GetFrameTime());
			Renderer::DrawText(OutputBuffer);
		}

		inline FrameTimer& GetFrameTimer() { return m_FrameTimer; }
		inline float GetFPS() { return m_FrameTimer.FPS(); }
		inline float GetFrameTime() { return m_FrameTimer.DeltaTime(); }

	private:
		FrameTimer m_FrameTimer;

	};

}