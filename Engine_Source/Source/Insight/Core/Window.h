#pragma once
#include <Engine_pch.h>

#include <Insight/Core.h>
#include "Insight/Events/Event.h"
#include "Insight/Input/Key_Codes.h"

class RenderingContext;

namespace Insight {


	using EventCallbackFn = std::function<void(Event&)>;

	struct WindowDescription
	{
		uint32_t Width;
		uint32_t Height;
		std::wstring Title;
		std::wstring Class;
		int NumCmdArgs;
		std::wstring CmdArgs;
		EventCallbackFn EventCallbackFunction;

		WindowDescription(EventCallbackFn CallbackFn, int CmdLineArgCount = 0, std::wstring CmdArgVals = L"", const std::wstring& title = L"Insight Editor", const std::wstring winClass = L"IE Class", uint32_t width = 1700, uint32_t height = 1000)
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
		
		inline void Resize(uint32_t NewWidth, uint32_t NewHeight, bool IsMinimized) 
		{ 
			IE_ASSERT(NewWidth > 0 && NewHeight > 0, "Window width and/or height cannot be zero.");
			
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
		virtual InputEventType GetAsyncKeyState(KeyMapCode Key) const = 0;
		virtual bool SetWindowTitle(const std::string& newText, bool completlyOverride = false) = 0;

		uint32_t GetWidth()								const { return m_LogicalWidth; }
		uint32_t GetHeight()							const { return m_LogicalHeight; }
		std::pair<uint32_t, uint32_t> GetDimensions()	const { return std::make_pair(m_LogicalWidth, m_LogicalHeight); }
		inline float GetAspectRatio()					const { return m_AspectRatio; }
		inline bool GetIsVisible()						const { return m_IsVisible; }
		inline bool GetIsVsyncEnabled()					const { return m_VSyncEnabled; }
		inline bool GetIsFullScreenEnabled()			const { return m_FullScreenEnabled; }
		inline void SetIsVisible(bool Visible)			{ m_IsVisible = Visible; }
		inline void SetVSyncEnabled(bool Enabled)		{ m_VSyncEnabled = Enabled; }
		inline void SetAspectRatio(float AspectRatio)	{ m_AspectRatio = AspectRatio; }
		virtual void SetFullScreenEnabled(bool Enabled) { m_FullScreenEnabled = Enabled; }

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

	protected:
		EventCallbackFn m_EventCallbackFn;
		
		float m_DPI = 96.0f;
		float m_AspectRatio = -1.0f;
		bool m_VSyncEnabled = false;
		bool m_FullScreenEnabled = false;
		bool m_IsVisible = true;
		uint32_t m_LogicalWidth = 0u;
		uint32_t m_LogicalHeight = 0u;
		std::wstring m_WindowTitle;
		std::wstring m_WindowClassName;
	};

}