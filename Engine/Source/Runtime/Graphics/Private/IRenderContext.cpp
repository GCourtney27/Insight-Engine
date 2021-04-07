#include <Engine_pch.h>

#include "Runtime/Graphics/Public/GraphicsCore.h"

#include "Runtime/Graphics/Public/IRenderContext.h"
#include "Runtime/Graphics/Public/ICommandContext.h"
#include "Runtime/Graphics/Private/IDevice.h"
#include "Runtime/Graphics/Private/ISwapChain.h"
#include "Runtime/Graphics/Private/ICommandManager.h"

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
			m_pWindow->AddFullScreenCallback(IE_BIND_LOCAL_EVENT_FN(IRenderContext::OnFullScreenToggled));
			m_pWindow->AddWindowResizeCallback(IE_BIND_LOCAL_EVENT_FN(IRenderContext::OnNativeResolutionChanged));
		}

		void IRenderContext::UnInitialize()
		{
			SAFE_DELETE_PTR(m_pDevice);
			SAFE_DELETE_PTR(g_pCommandManager);
			SAFE_DELETE_PTR(g_pContextManager);
			SAFE_DELETE_PTR(m_pSwapChain);
		}

		void IRenderContext::OnFullScreenToggled(bool FullScreenEnabled)
		{
			m_pSwapChain->ToggleFullScreen(FullScreenEnabled);
		}

		void IRenderContext::OnNativeResolutionChanged(const FVector2& NewResolution)
		{
			// TODO: Resize an UI contexts not necessarily the scene context.
			//		because th user can decide the scene render resolution to 
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
