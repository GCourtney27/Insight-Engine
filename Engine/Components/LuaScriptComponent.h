#pragma once
//#include "..\LUAScripting\LuaStateManager.h"
#include "Component.h"
#include "..\Framework\Singleton.h"
#include "Lua/LuaPlus/LuaPlus.h"

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

	bool Initialize(Entity* owner, std::string scriptFile);

	void Update(float deltaTime) override;
	void Destroy() override;
	void OnImGuiRender() override;

	void InitFromJSON(Entity* owner, const rapidjson::Value& componentInformation) override;

	bool lua_KeyIsPressed(int keycode);

	std::string& GetFilePath() { return filePath; }


private:
	//lua_State *L = nullptr;
	LuaPlus::LuaState* luaState = nullptr;
	LuaPlus::LuaObject Lua_UpdateFunction;
	std::string filePath;
	std::string cmd;
	int callCounter = 1;
};
