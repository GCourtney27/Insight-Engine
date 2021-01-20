//#include "Game.h"
//Game g_Game;

#if defined (IE_PLATFORM_BUILD_WIN32)
	#include <Windows.h>
	BOOL APIENTRY DllMain(HMODULE hModule,
		DWORD  ulReasonForCall,
		LPVOID lpReserved
	)
	{
		switch (ulReasonForCall)
		{
		case DLL_PROCESS_ATTACH:
			g_Game.OnDllLoad();
			break;
		case DLL_PROCESS_DETACH:
			g_Game.OnDllUnload();
			break;
		}
		return TRUE;
	}
#endif
