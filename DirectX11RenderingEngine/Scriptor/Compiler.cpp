#include "Compiler.h"
#include <iostream>

bool Compiler::Compile()
{
	// -- Create Library -- //
	system("C:/MinGW/bin/g++ -shared Scriptor/Scripts/ScriptedGameObject.cpp -o Scriptor/Scripts/CompiledBinaries/libSGO.so");

	// -- Load library -- //      
	fLib = dlopen("Scripts/CompiledBinaries/libSGO.so", RTLD_LAZY);
	if (!fLib)
	{
		std::cerr << "Cannot open library for .h file: " << dlerror() << '\n';

		//char* msg = dlerror();
		//std::string dlError(msg);
		//std::string error = "Cannot open library for .h file: " + dlError;
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

	if (fLib) {
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
