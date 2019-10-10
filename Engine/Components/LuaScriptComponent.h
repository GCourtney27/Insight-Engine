#pragma once
//#include "..\LUAScripting\LuaStateManager.h"
#include "Component.h"

extern "C"
{
#include "Lua535/lua.h";
#include "Lua535/lauxlib.h"
#include "Lua535/lualib.h"
}

class LuaScript : public Component
{
public:
	LuaScript(Entity* owner)
		: Component(owner) {}

	bool Initialize(std::string scriptFile);

	void Update() override;
	void Destroy() override;

	void OnImGuiRender() override;

	void SetCallCounter(int count) { callCounter = count; }

private:
	lua_State *L = nullptr;
	std::string filePath;
	std::string cmd;
	int callCounter = 1;
};
