#pragma once

#include <Insight/Core.h>

#include "Insight/Core/Window.h"

namespace Insight {


	struct UWPWindowDescription : WindowDescription
	{
		::IUnknown* pWindow;

		template <typename ... WindowDescriptionArgs>
		UWPWindowDescription(::IUnknown* pWindowHandle, WindowDescriptionArgs ... args)
			: pWindow(pWindowHandle), WindowDescription(args...) {}
	};

	class INSIGHT_API UWPWindow : public Window
	{
	public:
		UWPWindow(const UWPWindowDescription& props);
		virtual ~UWPWindow();

		void Init();

		virtual void OnUpdate() override {}
		virtual void Shutdown() override {}
		virtual void PostInit() override {}
		virtual bool ProccessWindowMessages() override { return true; }

		virtual void* GetNativeWindow() const override { return static_cast<void*>(m_pWindow); }
		virtual void CreateMessageBox(const std::wstring& Message, const std::wstring Title) override;

		virtual bool SetWindowTitleFPS(float fps) override { return false; }
		virtual void SetEventCallback(const EventCallbackFn& callback) override {}
		virtual bool SetWindowTitle(const std::string& newText, bool completlyOverride = false) { return false; }


	private:
		::IUnknown* m_pWindow;
	};
}
