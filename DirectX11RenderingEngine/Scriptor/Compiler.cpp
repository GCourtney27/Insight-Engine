#include "Compiler.h"
#include <iostream>
#include <stdlib.h>
#include <shellapi.h>
#include <Windows.h>

bool Compiler::Initialize(HWND windHandle)
{
	m_windHandle = windHandle;
	return true;
}

bool Compiler::Compile()
{
	//https://docs.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-shellexecutea?redirectedfrom=MSDN
	//https://stackoverflow.com/questions/6672257/how-to-properly-use-system-to-execute-a-command-in-c

	// -- Create Library -- //
	//system("C:\\MinGW\\bin\\g++ Scriptor\\Scripts\\CompiledBinaries\\libSGO.dll -o Scriptor\\Scripts\\ScriptedGameObject.cpp");
	//ShellExecuteExA("C:\\MinGW\\bin\\g++ -shared Scriptor\\Scripts\\ScriptedGameObject.cpp -o Scriptor\\Scripts\\CompiledBinaries\\libSGO.dll");
	
	LPCSTR operation = "open";
	LPCSTR file = "C:/MinGW/bin/g++";
	LPCSTR parameters = "-shared Scripts/ScriptedGameObject.cpp -o Scripts/CompiledBinaries/libSGO.dll";
	LPCSTR directory = "C:/VSDev/ComputerGraphics/DX11RenderingEngine/DirectX11RenderingEngine/Scriptor";

	
	//HINSTANCE hInst = ShellExecuteA(m_windHandle, operation, file, parameters, directory, SW_SHOWNORMAL);
	UINT res = WinExec("C:/MinGW/bin/g++ Scriptor/Scripts/ScriptedGameObject.cpp -o Scriptor/Scripts/CompiledBinaries/libSGO.dll", 1);

	// -- Load library -- //      
	fLib = dlopen("Scriptor/Scripts/CompiledBinaries/libSGO.dll", RTLD_LAZY);
	if (!fLib)
	{
		char* error = dlerror();
		//std::cerr << "Cannot open library for .cpp file: " << dlerror() << '\n';

		ErrorLogger::Log("Cannot open library from file");
		return false;
	}

	// -- Load function pointers -- //
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
		//Update = (VoidMethod_t)dlsym(fLib, "Update");
		Factory = (ScriptableGameObject)dlsym(fLib, "factory");

		compiledGO = Factory();



		// Add to vector in graphics

		// Run what is inside the functions
		//Simulate();
	}

	return true;
}
