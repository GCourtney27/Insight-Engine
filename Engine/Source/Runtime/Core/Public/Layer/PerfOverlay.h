#pragma once
#include "EngineDefines.h"

#include "Core/Public/Layer/Layer.h"

#include "Core/Public/Utility/FrameTimer.h"
#include "Graphics/Renderer.h"

namespace Insight {

	class INSIGHT_API PerfOverlay : public Layer
	{
	public:
		PerfOverlay() {}
		~PerfOverlay() {}

		virtual void OnAttach() override {}
		virtual void OnDetach() override {}
		virtual void OnImGuiRender() override {}
		

		virtual void OnUpdate(const float DeltaMs) override
		{
			m_FrameTimer.Tick();
			wchar_t OutputBuffer[256];
			swprintf_s(OutputBuffer, L"FPS: %f\nFrame Time: %fms", GetFPS(), GetFrameTime());
			Renderer::DrawText(OutputBuffer);
		}

		inline FrameTimer& GetFrameTimer() { return m_FrameTimer; }
		inline float GetFPS() { return m_FrameTimer.FPS(); }
		inline float GetFrameTime() { return m_FrameTimer.DeltaTime(); }

	private:
		FrameTimer m_FrameTimer;

	};

}