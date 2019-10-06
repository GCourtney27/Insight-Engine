#pragma once
#include <iostream>
#include <fstream>
#include "dlfcn.h"
#include <filesystem>
#include "CompilerHelper.h"
#include <string>

class Compiler
{
public: 
	Compiler() {}
	~Compiler() {}
	bool Initialize();


	bool CompileCPPFromFile(std::string fileToCompile);


private:
	void CreateConsoleWindow(int bufferLines, int bufferColumns, int windowLines, int windowColumns);
	void * fLib;
	VoidMethod_t pStart;
	VoidMethod_t pOnUpdate;
	ScriptableEntity_t Factory;
};