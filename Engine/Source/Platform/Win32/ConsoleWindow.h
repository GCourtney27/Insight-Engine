#pragma once

#include "EngineDefines.h"

#if IE_WIN32

#include <Windows.h>
#include "Math/Public/Vectors.h"

namespace Insight {

	/*
		Valid colors the console window text can be.
	*/
	enum EConsoleColor
	{
		CC_Black	= 0x0000,
		CC_Red		= 0x0004,
		CC_Green	= 0x0002,
		CC_Blue		= 0x0001,
		CC_Yellow	= CC_Red	| CC_Green,
		CC_Cyan		= CC_Green	| CC_Blue,
		CC_White	= CC_Red	| CC_Green | CC_Blue,
		CC_Magenta	= CC_Red	| CC_Blue,
		CC_Orange	= CC_Red	| CC_Yellow,
	};

	/*
		Describes a console window for writting messages to.
	*/
	struct ConsoleWindowDesc
	{
		// Wether the window is allowed to close.
		bool CanClose;
		// Output buffer dimensions.
		FVector2 BufferDims;
		// Window dimensions
		FVector2 WindowDims;
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