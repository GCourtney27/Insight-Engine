#include <Engine_pch.h>

#include "UWP_Window.h"

namespace Insight {

	UWPWindow::UWPWindow(const WindowDescription& props)
	{
		m_WindowTitle = props.Title;
		m_WindowClassName = props.Class;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
	}
	
	UWPWindow::~UWPWindow()
	{
	}
}
