#pragma once

#include <Insight/Core.h>

#include "Insight/Core/Window.h"
#include "Insight/Input/Key_Codes.h"


namespace Insight {

#if defined (IE_PLATFORM_BUILD_UWP)


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
		virtual void Shutdown() override {}
		virtual void PostInit() override {}
		virtual bool ProccessWindowMessages() override;

		virtual void* GetNativeWindow() const override { return static_cast<void*>(winrt::get_abi(*m_pCoreWindow)); }
		virtual void CreateMessageBox(const std::wstring& Message, const std::wstring& Title) override;
		
		virtual std::pair<uint32_t, uint32_t> GetDPI() const override
		{
			winrt::Windows::UI::Core::CoreWindow Window = winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread();
			auto Bounds = Window.Bounds();
			return std::make_pair(ConvertDipsToPixels(Bounds.Width), ConvertDipsToPixels(Bounds.Height));
		}

		virtual InputEventType GetAsyncKeyState(KeyMapCode Key) const override;
		virtual bool SetWindowTitleFPS(float fps) override { return false; }
		virtual bool SetWindowTitle(const std::string& newText, bool completlyOverride = false) { return false; }

	protected:
		// Keys
		void OnKeyDown_Callback(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::KeyEventArgs const& args);
		void OnKeyUp_Callback(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::KeyEventArgs const& args);
		// Mouse
		void OnMousePressed_Callback(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::PointerEventArgs const& args);
		void OnMouseReleased_Callback(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::PointerEventArgs const& args);
		void OnMouseMoved_Callback(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::PointerEventArgs const& args);



	private:
		bool m_MouseButtonPressStates[3];
		const uint8_t c_MouseButtonLeft = 0, c_MouseButtonRight = 1, c_MouseButtonMiddle = 2;

		const ::winrt::Windows::UI::Core::CoreWindow* m_pCoreWindow;
	};

#endif
}
