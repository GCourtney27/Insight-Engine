#pragma once
#include <Engine_pch.h>

#include <Runtime/Core.h>
#include "Runtime/Events/Event.h"
#include "Runtime/Input/KeyCodes.h"

class RenderingContext;

constexpr int cx_MaxLoadString = 100;

namespace Insight {


	enum EWindowMode
	{
		WM_Borderless = 1,
		WM_Windowed = 2,
		WM_FullScreen = WM_Borderless,
	};

	//
	//  Callback types
	//
	using EventCallbackFn = std::function<void(Event&)>;
	using WindowModeCallbackFn = std::function<void(EWindowMode)>;
	using WindowResizeCallbackFn = std::function<void(FVector2)>;

	struct WindowDescription
	{
		uint32_t Width;
		uint32_t Height;
		EString Title;
		EString Class;
		int NumCmdArgs;
		EString CmdArgs;
		EventCallbackFn EventCallbackFunction;

		WindowDescription(EventCallbackFn CallbackFn, int CmdLineArgCount = 0, std::wstring CmdArgVals = L"", const std::wstring& title = L"Insight Ed", const std::wstring winClass = L"CLASS", uint32_t width = 1700, uint32_t height = 1000)
			: EventCallbackFunction(CallbackFn), NumCmdArgs(CmdLineArgCount), CmdArgs(CmdArgVals), Title(title), Class(winClass), Width(width), Height(height)
		{
		}
	};

	class INSIGHT_API Window
	{
	public:

		virtual ~Window() {}

		virtual void OnUpdate() = 0;
		virtual void BackgroundUpdate() {}
		virtual void Shutdown() = 0;
		virtual void PostInit() = 0;

		virtual bool ProccessWindowMessages() = 0;
		
		inline void Resize(Int32 NewWidth, Int32 NewHeight, bool IsMinimized) 
		{ 
			IE_ASSERT(NewWidth > 0.f && NewHeight > 0.f, "Window width and/or height cannot be zero.");
			
			m_LogicalWidth	= ConvertDipsToPixels((float)NewWidth);
			m_LogicalHeight = ConvertDipsToPixels((float)NewHeight);
			m_AspectRatio	= static_cast<float>(NewWidth) / static_cast<float>(NewHeight); 
			m_IsVisible		= !IsMinimized;

			SetNativeWindowDPI();
		}
		virtual void CreateMessageBox(const std::wstring& Message, const std::wstring& Title) = 0;

		template <typename DataType = float>
		inline DataType GetDPI() const { return static_cast<DataType>(m_DPI); }
		virtual void* GetNativeWindow() const = 0;
		virtual bool SetWindowTitleFPS(float fps) = 0;
		virtual EInputEventType GetAsyncKeyState(KeyMapCode Key) const = 0;
		virtual bool SetWindowTitle(const std::string& newText, bool completlyOverride = false) = 0;

		Int32 GetWidth()								const { return m_LogicalWidth; }
		Int32 GetHeight()								const { return m_LogicalHeight; }
		FVector2 GetDimensions()						const { return FVector2((float)m_LogicalWidth, (float)m_LogicalHeight); }
		FORCE_INLINE float GetAspectRatio()				const { return m_AspectRatio; }
		FORCE_INLINE bool GetIsVisible()				const { return m_IsVisible; }
		FORCE_INLINE bool GetIsFullScreenActive()		const { return m_WindowMode == EWindowMode::WM_Borderless; }
		FORCE_INLINE EWindowMode GetWindowMode()			{ return m_WindowMode;}
		FORCE_INLINE void SetIsVisible(bool Visible)		{ m_IsVisible = Visible; }
		FORCE_INLINE void SetAspectRatio(float AspectRatio)	{ m_AspectRatio = AspectRatio; }
		FORCE_INLINE void SetWindowMode(EWindowMode Mode);
		FORCE_INLINE void SetDPI(float NewDPI)				{ m_DPI = NewDPI; Resize(m_LogicalWidth, m_LogicalHeight, !m_IsVisible); }

		FORCE_INLINE void AttachWindowModeChangedCallback(WindowModeCallbackFn Fn) { m_WindowModeChangedCallbacks.push_back(Fn); }
		FORCE_INLINE void AttachWindowResizeCallback(WindowResizeCallbackFn Fn) { m_WindowResizeCallbacks.push_back(Fn); }

		inline void SetEventCallback(const EventCallbackFn& callback) { m_EventCallbackFn = callback; }
		EventCallbackFn& GetEventCallbackFn() { return m_EventCallbackFn; }

		inline int ConvertDipsToPixels(float dips) const noexcept
		{
			return int(dips * m_DPI / 96.0f + 0.5f);
		}

		inline float ConvertPixelsToDips(int pixels) const noexcept
		{
			return (float(pixels) * 96.0f / m_DPI);
		}

	protected:
		virtual void SetNativeWindowDPI() = 0;
		virtual void OnWindowModeChanged() = 0;

	protected:
		EventCallbackFn m_EventCallbackFn;
		
		float m_DPI = 96.0f;
		float m_AspectRatio = -1.0f;
		bool m_IsVisible = true;
		EWindowMode m_WindowMode = EWindowMode::WM_Windowed;
		Int32 m_LogicalWidth = 0u;
		Int32 m_LogicalHeight = 0u;
		
		std::vector<WindowModeCallbackFn> m_WindowModeChangedCallbacks;
		std::vector<WindowResizeCallbackFn> m_WindowResizeCallbacks;

		EString m_WindowTitle;
		EString m_WindowClassName;
	};


	//
	// Inline function implementations
	//
	void Window::SetWindowMode(EWindowMode Mode)
	{
		m_WindowMode = Mode; 

		// Let the platform window account for changes.
		//
		OnWindowModeChanged();

		// Invoke the callbacks.
		//
		for (auto& Callback : m_WindowModeChangedCallbacks)
			Callback(m_WindowMode);
	}

}