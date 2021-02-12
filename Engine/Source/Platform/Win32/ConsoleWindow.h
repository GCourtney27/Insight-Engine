#pragma once

#include <Runtime/CoreMacros.h>

#if IE_PLATFORM_BUILD_WIN32

#include "Runtime/Math/ie_Vectors.h"

namespace Insight {

	/*
		Valid colors the console window text can be.
	*/
	enum EConsoleColor
	{
		CC_Black	= 0,
		CC_Red		= FOREGROUND_RED,
		CC_Green	= FOREGROUND_GREEN,
		CC_Blue		= FOREGROUND_BLUE,
		CC_Yellow	= FOREGROUND_RED	| FOREGROUND_GREEN,
		CC_Cyan		= FOREGROUND_GREEN	| FOREGROUND_BLUE,
		CC_White	= FOREGROUND_RED	| FOREGROUND_GREEN | FOREGROUND_BLUE,
		CC_Magenta	= FOREGROUND_RED	| FOREGROUND_BLUE,
		CC_Orange	= CC_Red | CC_Yellow,
	};

	/*
		Describes a console window for writting messages to.
	*/
	struct ConsoleWindowDesc
	{
		// Wether the window is allowed to close.
		bool CanClose;
		// Output buffer dimensions.
		Math::ieFloat2 BufferDims;
		// Window dimensions
		Math::ieFloat2 WindowDims;
		// The default text color when adding text to the log.
		EConsoleColor DefaultForegroundColor;
		// The name of this logger.
		std::string LoggerName;
	};

	class INSIGHT_API ConsoleWindow
	{
	public:
		ConsoleWindow();
		~ConsoleWindow();
		
		/*
			Initialized the console window.
			@param Desc - Description describing how the window shoule be created.
		*/
		bool Create(const ConsoleWindowDesc& Desc);
		
		/*
			Returns the descriptoin the window was created with.
		*/
		inline const ConsoleWindowDesc& GetDesc() const { return m_Desc; }

		/*
			Returns the handle to the window.
		*/
		inline HWND		GetWindowHandle()	const { return m_WindowHandle; }

		/*
			Returns the handle to the menu bar of the window.
		*/
		inline HMENU	GetMenuHandle()		const { return m_WindowHMenu; }

		/*
			Returns a handle to the printable surface of the window.
		*/
		inline HANDLE	GetOutputHandle()	const { return m_OutputHandle; }

		/*
			Returns the name of the logger.
		*/
		inline const std::string& GetName() const { return m_Desc.LoggerName; }

		/*
			Sets the color of the logger text on screen. All messages proceeding this call 
			will be in the specified color. Call ConsoleWindow::ResetColors to revert back to defaults.
			@param NewColor - An enum value specifying the new color to change too.
		*/
		inline void SetForegroundColor(EConsoleColor NewColor)
		{
			SetConsoleTextAttribute(GetOutputHandle(), static_cast<WORD>(NewColor));
		}

		/*
			Resets the output colors to the windows description's specified default value.
		*/
		void ResetColors()
		{
			SetForegroundColor(m_Desc.DefaultForegroundColor);
		}

	protected:
		/*
			Cleanup the window and release the handles to it.
		*/
		void Destroy();

	protected:
		ConsoleWindowDesc m_Desc;
		HWND m_WindowHandle;
		HMENU m_WindowHMenu;
		HANDLE m_OutputHandle;
	};

}

#endif