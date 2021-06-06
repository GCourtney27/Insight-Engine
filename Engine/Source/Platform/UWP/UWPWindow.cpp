#include <Engine_pch.h>

#if IE_UWP_DESKTOP

#include "UWPWindow.h"

#include "Core/Events/KeyEvent.h"
#include "Core/Events/MouseEvent.h"
#include "Core/Events/ApplicationEvent.h"
#include "Core/Exception.h"

namespace Insight {

	UWPWindow::UWPWindow(const UWPWindowDescription& Desc)
	{
		IE_ASSERT(Desc.Class != L"", "Window class name cannot be blank.");
		IE_ASSERT(Desc.Title != L"", "Window title cannot be blank.");
		IE_ASSERT(Desc.Width > 0 && Desc.Height > 0, "Width and height of window cannot be 0.");

		m_WindowTitle = Desc.Title;
		m_WindowClassName = Desc.Class;
		m_LogicalWidth = Desc.Width;
		m_LogicalHeight = Desc.Height;
		m_pCoreWindow = Desc.pWindow;
		m_EventCallbackFn = Desc.EventCallbackFunction;

		Init();
	}

	UWPWindow::~UWPWindow()
	{
	}

	void UWPWindow::Init()
	{
		for (int i = 0; i < 3; ++i)
			m_MouseButtonPressStates[i] = false;

		// Input
		m_pCoreWindow->KeyDown({ this, &UWPWindow::OnKeyDown_Callback });
		m_pCoreWindow->KeyUp({ this, &UWPWindow::OnKeyUp_Callback });
		m_pCoreWindow->PointerPressed({ this, &UWPWindow::OnMousePressed_Callback });
		m_pCoreWindow->PointerReleased({ this, &UWPWindow::OnMouseReleased_Callback });
		m_pCoreWindow->PointerMoved({ this, &UWPWindow::OnMouseMoved_Callback });

		// App
		m_pCoreWindow->Closed(
			[this](auto&&, auto&&)
			{
				WindowCloseEvent event;
				GetEventCallbackFn()(event);
			}
		);

		m_pCoreWindow->SizeChanged({ this, &UWPWindow::OnWindowSizeChanged_Callback });
		
		winrt::Windows::ApplicationModel::Core::CoreApplication::Suspending({ this, &UWPWindow::OnSuspending });
		winrt::Windows::ApplicationModel::Core::CoreApplication::Resuming({ this, &UWPWindow::OnResuming });

	}

	bool UWPWindow::ProccessWindowMessages()
	{
		return true;
	}

	void UWPWindow::OnUpdate()
	{
		using namespace winrt::Windows::UI::Core;
		CoreWindow::GetForCurrentThread().Dispatcher().ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
	}

	void UWPWindow::BackgroundUpdate()
	{
		using namespace winrt::Windows::UI::Core;
		CoreWindow::GetForCurrentThread().Dispatcher().ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
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

	void UWPWindow::OnWindowSizeChanged_Callback(winrt::Windows::UI::Core::CoreWindow const& Sender, winrt::Windows::UI::Core::WindowSizeChangedEventArgs const& Args)
	{
		uint32_t Width = static_cast<uint32_t>(Sender.Bounds().Width);
		uint32_t Height = static_cast<uint32_t>(Sender.Bounds().Height);
		Resize(Width, Height, !m_IsVisible);

		// Notify
		WindowResizeEvent e(Width, Height, !m_IsVisible);
		GetEventCallbackFn()(e);
	}

	void UWPWindow::OnDpiChanged(winrt::Windows::Graphics::Display::DisplayInformation const& Sender, winrt::Windows::Foundation::IInspectable const& Args)
	{
		SetDPI(Sender.LogicalDpi());
		
		// Notify
		WindowResizeEvent e(m_LogicalWidth, m_LogicalHeight, !m_IsVisible);
		GetEventCallbackFn()(e);
	}

	void UWPWindow::OnSuspending(winrt::Windows::Foundation::IInspectable const& /*sender*/, winrt::Windows::ApplicationModel::SuspendingEventArgs const& args)
	{
		auto Deferral = args.SuspendingOperation().GetDeferral();

		auto d = std::async(std::launch::async, [this, Deferral]()
			{
				AppSuspendingEvent e;
				GetEventCallbackFn()(e);

				Deferral.Complete();
			});
	}

	void UWPWindow::OnResuming(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable const& args)
	{
		AppResumingEvent e;
		GetEventCallbackFn()(e);
	}

}
#endif // IE_UWP_DESKTOP
