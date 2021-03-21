#include <Engine_pch.h>

#include "Runtime/Graphics/Public/IRenderContext.h"
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
			m_pWindow->AddFullScreenCallback(IE_BIND_LOCAL_EVENT_FN(IRenderContext::OnFullScreenToggled));
		}

		void IRenderContext::UnInitialize()
		{
			SAFE_DELETE_PTR(m_pDevice);
			SAFE_DELETE_PTR(m_pSwapChain);
			SAFE_DELETE_PTR(m_pCommandManager);
		}

		void IRenderContext::OnFullScreenToggled(bool FullScreenEnabled)
		{
			m_pSwapChain->ToggleFullScreen(FullScreenEnabled);
		}

		void IRenderContext::EnableVSync(bool VsyncEnabled)
		{
			m_pSwapChain->ToggleVsync(VsyncEnabled);
		}
	}
}
