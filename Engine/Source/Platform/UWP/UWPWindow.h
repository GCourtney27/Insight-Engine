#pragma once

#include "EngineDefines.h"

#include "Core/Window.h"
#include "Input/KeyCodes.h"


namespace Insight {

#if IE_UWP_DESKTOP


	struct UWPWindowDescription : WindowDescription
	{
		const ::winrt::Windows::UI::Core::CoreWindow* pWindow;

		template <typename ... WindowDescriptionArgs>
		UWPWindowDescription(const ::winrt::Windows::UI::Core::CoreWindow& pWindowHandle, WindowDescriptionArgs ... args)
			: pWindow(&pWindowHandle), WindowDescription(args...) {}
	};


	class INSIGHT_API UWPWindow : public Window
	{
	public:
		UWPWindow(const UWPWindowDescription& props);
		virtual ~UWPWindow();

		void Init();

		virtual void OnUpdate() override;
		virtual void BackgroundUpdate() override;
		virtual void Shutdown() override {}
		virtual void PostInit() override {}
		virtual bool ProccessWindowMessages() override;

		virtual void* GetNativeWindow() const override { return static_cast<void*>(winrt::get_abi(*m_pCoreWindow)); }

		virtual bool SetWindowTitle(const std::string& newText, bool completlyOverride = false) { return false; }

	protected:
		virtual inline void SetNativeWindowDPI() override
		{
			using namespace winrt::Windows::Graphics::Display;
			m_DPI = DisplayInformation::GetForCurrentView().LogicalDpi();
		}

		// Keys
		void OnKeyDown_Callback(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::KeyEventArgs const& args);
		void OnKeyUp_Callback(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::KeyEventArgs const& args);
		// Mouse
		void OnMousePressed_Callback(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::PointerEventArgs const& args);
		void OnMouseReleased_Callback(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::PointerEventArgs const& args);
		void OnMouseMoved_Callback(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::PointerEventArgs const& args);

		// Application
		// Window
		void OnWindowSizeChanged_Callback(winrt::Windows::UI::Core::CoreWindow const& Sender, winrt::Windows::UI::Core::WindowSizeChangedEventArgs const& Args);
		void OnDpiChanged(winrt::Windows::Graphics::Display::DisplayInformation const& Sender, winrt::Windows::Foundation::IInspectable const& Args);
		void OnSuspending(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::ApplicationModel::SuspendingEventArgs const& args);
		void OnResuming(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable const& args);

	private:
		bool m_MouseButtonPressStates[3];
		const uint8_t c_MouseButtonLeft = 0, c_MouseButtonRight = 1, c_MouseButtonMiddle = 2;

		/*
			A reference to the main window. Not thread safe.
		*/
		const ::winrt::Windows::UI::Core::CoreWindow* m_pCoreWindow;
	};

#endif
}
