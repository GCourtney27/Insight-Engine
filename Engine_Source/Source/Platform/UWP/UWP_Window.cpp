#include <Engine_pch.h>

#if defined (IE_PLATFORM_BUILD_UWP)

#include "UWP_Window.h"

#include "Insight/Events/Key_Event.h"
#include "Insight/Events/Mouse_Event.h"
#include "Insight/Events/Application_Event.h"
#include "Insight/Core/ie_Exception.h"

namespace Insight {

	UWPWindow::UWPWindow(const UWPWindowDescription& Desc)
	{
		IE_ASSERT(Desc.Class != L"", "Window class name cannot be blank.");
		IE_ASSERT(Desc.Title != L"", "Window title cannot be blank.");
		IE_ASSERT(Desc.Width > 0 && Desc.Height > 0, "Width and height of window cannot be 0.");

		m_WindowTitle		= Desc.Title;
		m_WindowClassName	= Desc.Class;
		m_WindowWidth		= Desc.Width;
		m_WindowHeight		= Desc.Height;
		m_pCoreWindow		= Desc.pWindow;
		m_EventCallbackFn	= Desc.EventCallbackFunction;

		Init();
	}
	
	UWPWindow::~UWPWindow()
	{
	}

	void UWPWindow::Init()
	{
		for (int i = 0; i < 3; ++i)
			m_MouseButtonPressStates[i] = false;

		m_pCoreWindow->KeyDown({ this, &UWPWindow::OnKeyDown_Callback });
		m_pCoreWindow->KeyUp({ this, &UWPWindow::OnKeyUp_Callback });
		m_pCoreWindow->PointerPressed({ this, &UWPWindow::OnMousePressed_Callback });
		m_pCoreWindow->PointerReleased({ this, &UWPWindow::OnMouseReleased_Callback });
		m_pCoreWindow->PointerMoved({ this, &UWPWindow::OnMouseMoved_Callback });
	}

	bool UWPWindow::ProccessWindowMessages()
	{
		return true;
	}

	void UWPWindow::CreateMessageBox(const std::wstring& Message, const std::wstring& Title)
	{
		
	}

	void UWPWindow::OnUpdate()
	{
	}

	InputEventType UWPWindow::GetAsyncKeyState(KeyMapCode Key) const
	{
		const winrt::Windows::System::VirtualKey VirtualKey = (winrt::Windows::System::VirtualKey)Key;
		auto state = winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().GetAsyncKeyState(VirtualKey);
		if (state == winrt::Windows::UI::Core::CoreVirtualKeyStates::Down)
			return InputEventType_Pressed;
		else
			return InputEventType_Released;
	}


	// ------------------
	//		Callbacks	 |
	// ------------------

	void UWPWindow::OnKeyDown_Callback(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::KeyEventArgs const& args)
	{
		auto key = args.VirtualKey();
		KeyPressedEvent event((KeyMapCode)((char)key), 0);
		m_EventCallbackFn(event);
	}

	void UWPWindow::OnKeyUp_Callback(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::KeyEventArgs const& args)
	{
		auto key = args.VirtualKey();
		KeyReleasedEvent event((KeyMapCode)((char)key));
		m_EventCallbackFn(event);
	}

	void UWPWindow::OnMousePressed_Callback(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::PointerEventArgs const& args)
	{
		auto CPProps = args.CurrentPoint().Properties();

		if (!m_MouseButtonPressStates[c_MouseButtonLeft] && CPProps.IsLeftButtonPressed())
		{
			m_MouseButtonPressStates[c_MouseButtonLeft] = true;
			MouseButtonPressedEvent event(KeyMapCode_Mouse_Button_Left);
			m_EventCallbackFn(event);
		}

		if (!m_MouseButtonPressStates[c_MouseButtonRight] && CPProps.IsRightButtonPressed())
		{
			m_MouseButtonPressStates[c_MouseButtonRight] = true;
			MouseButtonPressedEvent event(KeyMapCode_Mouse_Button_Right);
			m_EventCallbackFn(event);
		}

		if (!m_MouseButtonPressStates[c_MouseButtonMiddle] && CPProps.IsMiddleButtonPressed())
		{
			m_MouseButtonPressStates[c_MouseButtonMiddle] = true;
			MouseButtonPressedEvent event(KeyMapCode_Mouse_Button_Middle);
			m_EventCallbackFn(event);
		}
		
		//CPProps.MouseWheelDelta();
	}

	void UWPWindow::OnMouseReleased_Callback(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::PointerEventArgs const& args)
	{
		auto CPProps = args.CurrentPoint().Properties();

		if (m_MouseButtonPressStates[c_MouseButtonLeft] && !CPProps.IsLeftButtonPressed())
		{
			m_MouseButtonPressStates[c_MouseButtonLeft] = false;

			MouseButtonPressedEvent event(KeyMapCode_Mouse_Button_Left);
			m_EventCallbackFn(event);
		}

		if (m_MouseButtonPressStates[c_MouseButtonRight] && !CPProps.IsRightButtonPressed())
		{
			m_MouseButtonPressStates[c_MouseButtonRight] = false;

			MouseButtonPressedEvent event(KeyMapCode_Mouse_Button_Right);
			m_EventCallbackFn(event);
		}

		if (m_MouseButtonPressStates[c_MouseButtonMiddle] && !CPProps.IsMiddleButtonPressed())
		{
			m_MouseButtonPressStates[c_MouseButtonMiddle] = false;
			
			MouseButtonPressedEvent event(KeyMapCode_Mouse_Button_Middle);
			m_EventCallbackFn(event);
		}
	}

	void UWPWindow::OnMouseMoved_Callback(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::PointerEventArgs const& args)
	{
		MouseMovedEvent event(args.CurrentPoint().Position().X, args.CurrentPoint().Position().Y, (KeyMapCode)(KeyMapCode_Mouse_MoveX | KeyMapCode_Mouse_MoveY));
		m_EventCallbackFn(event);
	}

}

#endif // IE_PLATFORM_BUILD_UWP
