#include <Engine_pch.h>

#include "UWP_Window.h"

namespace Insight {

	UWPWindow::UWPWindow(const UWPWindowDescription& Desc)
	{
		IE_ASSERT(Desc.Class != L"", "Window class name cannot be blank");
		IE_ASSERT(Desc.Title != L"", "Window title cannot be blank.");
		IE_ASSERT(Desc.Width > 0 && Desc.Height > 0, "Width and height of window cannot be 0.");

		m_WindowTitle = Desc.Title;
		m_WindowClassName = Desc.Class;
		m_WindowWidth = Desc.Width;
		m_WindowHeight = Desc.Height;
		m_pWindow = Desc.pWindow;
		m_EventCallbackFn = Desc.EventCallbackFunction;

	}
	
	UWPWindow::~UWPWindow()
	{
	}

	void UWPWindow::Init()
	{

	}

}
