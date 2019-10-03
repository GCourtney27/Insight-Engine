#pragma once
#include <iostream>
#include <fstream>
#include "dlfcn.h"
#include <filesystem>
#include "CompilerHelper.h"




class Compiler
{
public: 
	Compiler() {}
	~Compiler() {}

	bool Initialize(HWND windHandle);

	bool Compile();

	BaseScriptableGameObject* compiledGO = nullptr;
private:
	void * fLib;
	VoidMethod_t Start;
	VoidMethod_t Update;
	ScriptableGameObject Factory;
	HWND m_windHandle;
};