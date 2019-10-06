#include "Compiler.h"
#include <iostream>
#include <stdlib.h>
#include <shellapi.h>
#include <Windows.h>
#include <stdio.h>

bool Compiler::Initialize()
{
	CreateConsoleWindow(500, 120, 32, 120);
	return true;
}

bool Compiler::CompileCPPFromFile(std::string fileToCompile)
{
	// -- Known Issues with this method-- \\
	// x64
	//		Entity.h included in FileEntity.h
	//		- Error: No such file or directory for <DirectXMath.h>
	//			- Fails to compile .dll
	//		Entity.h not included
	//		- Error: Scriptor/Scripts/CompiledBinaries/libEntity.dll is not a valid Win32 application
	//			- Success Compiles .dll
	// x86 (Light failed to compile | Multiple heap allignment warning)
	//		Entity.h included in FileEntity.h
	//		- Error(Same as x64): No such file or directory for <DirectXMath.h>
	//			- Fails to compile .dll
	//		Entity.h not included
	//		- Success no errors
	//			- Success Compiles .dll


	//UINT result = WinExec("C:/MinGW/bin/g++ -shared Scriptor/Scripts/FileEntity.cpp -o Scriptor/Scripts/CompiledBinaries/libEntity.dll", 1);
	
	system("C:/MinGW/bin/g++ -shared Scriptor/Scripts/FileEntity.cpp -o Scriptor/Scripts/CompiledBinaries/libEntity.dll");
	
	// -- Load library -- //      
	fLib = dlopen("Scriptor/Scripts/CompiledBinaries/libEntity.dll", RTLD_LAZY);
	if (!fLib)
	{
		char* error = dlerror();
		//std::cerr << "Cannot open library for .cpp file: " << dlerror() << '\n';

		printf(error);
		return false;
	}
	const char *dlsym_error = dlerror();
	if (dlsym_error)
	{
		//cerr << "Cannot load symbol 'VoidMethod_t': " << dlsym_error << '\n';
		ErrorLogger::Log("Cannot load symbol for one or more methods");
		dlclose(fLib);
		return false;
	}

	if (fLib)
	{
		//Start = (VoidMethod_t)dlsym(fLib, "Start");
		//OnUpdatePointer = (VoidMethod_t)dlsym(fLib, "Update");
		Factory = (ScriptableEntity_t)dlsym(fLib, "factory");

		//compiledGO = Factory();

	}

	return false;
}

void Compiler::CreateConsoleWindow(int bufferLines, int bufferColumns, int windowLines, int windowColumns)
{
	// Our temp console info struct
	CONSOLE_SCREEN_BUFFER_INFO coninfo;

	// Get the console info and set the number of lines
	AllocConsole();
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.Y = bufferLines;
	coninfo.dwSize.X = bufferColumns;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

	SMALL_RECT rect;
	rect.Left = 0;
	rect.Top = 0;
	rect.Right = windowColumns;
	rect.Bottom = windowLines;
	SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &rect);

	FILE *stream;
	freopen_s(&stream, "CONIN$", "r", stdin);
	freopen_s(&stream, "CONOUT$", "w", stdout);
	freopen_s(&stream, "CONOUT$", "w", stderr);

	// Prevent accidental console window close
	HWND consoleHandle = GetConsoleWindow();
	HMENU hmenu = GetSystemMenu(consoleHandle, FALSE);
	EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED);
}
