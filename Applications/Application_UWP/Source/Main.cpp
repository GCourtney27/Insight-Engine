//
// Main.cpp
//

#include "pch.h"
#include "UWP_Client_App.h"


using namespace winrt::Windows::ApplicationModel;
using namespace winrt::Windows::ApplicationModel::Core;
using namespace winrt::Windows::ApplicationModel::Activation;
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::UI::Input;
using namespace winrt::Windows::UI::Popups;
using namespace winrt::Windows::UI::ViewManagement;
using namespace winrt::Windows::System;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Graphics::Display;
using namespace DirectX;

class ViewProvider : public winrt::implements<ViewProvider, IFrameworkView>
{
public:
    ViewProvider() noexcept
    {
        //LoadPackagedLibrary()
    }

    // IFrameworkView methods
    void Initialize(CoreApplicationView const & applicationView)
    {
        applicationView.Activated({ this, &ViewProvider::OnActivated });

        m_pApp = Insight::CreateApplication();
    }

    void Uninitialize() noexcept
    {
        m_pApp.reset();
    }

    void SetWindow(CoreWindow const & window)
    {
       /* winrt::Windows::System::VirtualKey k = winrt::Windows::System::VirtualKey::A;
        auto state = window.GetAsyncKeyState(k);
        
        if (state == winrt::Windows::UI::Core::CoreVirtualKeyStates::Down)
        {

        }*/

        auto dispatcher = CoreWindow::GetForCurrentThread().Dispatcher();

        dispatcher.AcceleratorKeyActivated({ this, &ViewProvider::OnAcceleratorKeyActivated });

        auto navigation = SystemNavigationManager::GetForCurrentView();

        // UWP on Xbox One triggers a back request whenever the B button is pressed
        // which can result in the app being suspended if unhandled
        navigation.BackRequested([](const winrt::Windows::Foundation::IInspectable&, const BackRequestedEventArgs& args)
        {
            args.Handled(true);
        });

        DisplayInformation::DisplayContentsInvalidated({ this, &ViewProvider::OnDisplayContentsInvalidated });


        // ! IMPORTANT !
        // In order to keep an instance of the main window available 
        // for all threads we make a static referene here so the CoreWindow
        // does not get destroyed. We cannot call 'CoreWindow::GetForCurrentThread'
        // on the graphics thread for example.
        static CoreWindow s_Window = CoreWindow::GetForCurrentThread();
        Insight::UWPWindowDescription WindowDesc(s_Window, IE_BIND_EVENT_FN(Insight::Application::OnEvent, m_pApp.get()));
        m_pUWPWindow = std::make_shared<Insight::UWPWindow>(WindowDesc);
        m_pApp->SetWindow(m_pUWPWindow);
        m_pApp->Initialize();
    }

    void Load(winrt::hstring const&) noexcept
    {
    }

    void Run()
    {
        Insight::WindowResizeEvent e(m_pUWPWindow->GetWidth(), m_pUWPWindow->GetHeight(), false);
        m_pUWPWindow->GetEventCallbackFn()(e);

        m_pApp->Run();
    }

protected:
    // Event handlers
    void OnActivated(CoreApplicationView const & /*applicationView*/, IActivatedEventArgs const & args)
    {
        if (args.Kind() == ActivationKind::Launch)
        {
            auto launchArgs = (const LaunchActivatedEventArgs*)(&args);

            if (launchArgs->PrelaunchActivated())
            {
                // Opt-out of Prelaunch
                CoreApplication::Exit();
                return;
            }
        }

        // Change to ApplicationViewWindowingMode::FullScreen to default to full screen or PreferredLaunchViewSize for windowed
        ApplicationView::PreferredLaunchWindowingMode(ApplicationViewWindowingMode::PreferredLaunchViewSize);
        
        auto desiredSize = Size((float)m_pUWPWindow->GetWidth(), (float)m_pUWPWindow->GetHeight());

        ApplicationView::PreferredLaunchViewSize(desiredSize);

        auto view = ApplicationView::GetForCurrentView();

        auto minSize = Size(320, 200);

        view.SetPreferredMinSize(minSize);

        CoreWindow::GetForCurrentThread().Activate();

        view.FullScreenSystemOverlayMode(FullScreenSystemOverlayMode::Minimal);

        view.TryResizeView(desiredSize);
    }

    void OnAcceleratorKeyActivated(CoreDispatcher const &, AcceleratorKeyEventArgs const & args)
    {
        if (args.EventType() == CoreAcceleratorKeyEventType::SystemKeyDown
            && args.VirtualKey() == VirtualKey::Enter
            && args.KeyStatus().IsMenuKeyDown
            && !args.KeyStatus().WasKeyDown)
        {
            // Implements the classic ALT+ENTER fullscreen toggle
            auto view = ApplicationView::GetForCurrentView();

            if (view.IsFullScreenMode())
                view.ExitFullScreenMode();
            else
                view.TryEnterFullScreenMode();

            args.Handled(true);
        }
    }

    void OnDisplayContentsInvalidated(DisplayInformation const & /*sender*/, IInspectable const & /*args*/)
    {
        //m_game->ValidateDevice();
    }

private:
    std::unique_ptr<Insight::Application> m_pApp;
    std::shared_ptr<Insight::UWPWindow> m_pUWPWindow;

};

class ViewProviderFactory : public winrt::implements<ViewProviderFactory, IFrameworkViewSource>
{
public:
    IFrameworkView CreateView()
    {
        return winrt::make<ViewProvider>();
    }
};


// Entry point
int WINAPI wWinMain(
    _In_ HINSTANCE /*hInstance*/,
    _In_ HINSTANCE /*hPrevInstance*/,
    _In_ LPWSTR    /*lpCmdLine*/,
    _In_ int       /*nCmdShow*/
)
{
    if (!XMVerifyCPUSupport())
    {
        throw std::exception("XMVerifyCPUSupport");
    }

    auto viewProviderFactory = winrt::make<ViewProviderFactory>();
    CoreApplication::Run(viewProviderFactory);
    return 0;
}


// Exit helper
void ExitGame() noexcept
{
    winrt::Windows::ApplicationModel::Core::CoreApplication::Exit();
}
