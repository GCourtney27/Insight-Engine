#include <Engine_pch.h>

#include "Runtime/Graphics/Public/IRenderContext.h"
#include "Runtime/Graphics/Public/ICommandContext.h"
#include "Runtime/Graphics/Public/IDevice.h"
#include "Runtime/Graphics/Private/ISwapChain.h"
#include "Runtime/Graphics/Private/ICommandManager.h"
#include "Runtime/Graphics/Public/IGeometryBufferManager.h"
#include "Runtime/Graphics/Public/ResourceManagement/ITextureManager.h"

#include "Runtime/Core/Window.h"


namespace Insight
{
	namespace Graphics
	{

		void IRenderContext::SetWindow(std::shared_ptr<Window> pWindow)
		{
			IE_ASSERT(pWindow.get() != NULL);
			m_pWindow = pWindow;

			// Setup event callbacks
			//
			m_pWindow->AttachWindowModeChangedCallback(IE_BIND_LOCAL_EVENT_FN(IRenderContext::OnWindowModeChanged));
			m_pWindow->AttachWindowResizeCallback(IE_BIND_LOCAL_EVENT_FN(IRenderContext::OnNativeResolutionChanged));
		}

		void IRenderContext::UnInitialize()
		{
			g_pCommandManager->IdleGPU();
		}

		void IRenderContext::OnWindowModeChanged(EWindowMode Mode)
		{
			m_pSwapChain->ToggleFullScreen(Mode == EWindowMode::WM_FullScreen);
		}

		void IRenderContext::OnNativeResolutionChanged(const FVector2& NewResolution)
		{
			// TODO: Resize UI contexts not necessarily the scene context.
			//		because the user can decide the scene render resolution to 
			//		save performance.
			//m_pSwapChain->Resize(NewResolution);
		}

		void IRenderContext::EnableVSync(bool VsyncEnabled)
		{
			m_pSwapChain->ToggleVsync(VsyncEnabled);
		}

		void IRenderContext::Present()
		{
			// TODO: Stall until frame n-1 is complete.
			m_pSwapChain->SwapBuffers();
		}
	}
}
