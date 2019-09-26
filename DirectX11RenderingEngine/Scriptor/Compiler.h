#pragma once
#include <iostream>
#include <fstream>
#include "dlfcn.h"
#include <stdlib.h>
#include <filesystem>
#include "CompilerHelper.h"




class Compiler
{
public: 
	Compiler() {}
	~Compiler() {}

	bool Compile();

	BaseScriptableGameObject* compiledGO = nullptr;
private:
	void * fLib;
	VoidMethod_t Start;
	VoidMethod_t Update;
	ScriptableGameObject Factory;
};