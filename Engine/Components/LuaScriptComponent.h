#pragma once
//#include "..\LUAScripting\LuaStateManager.h"
#include "Component.h"
#include "..\Framework\Singleton.h"
#include "Lua/LuaPlus/LuaPlus.h"

extern "C"
{
	#include "Lua535/lua.h"
	#include "Lua535/lauxlib.h"
	#include "Lua535/lualib.h"
}

class LuaScript : public Component
{
public:

	LuaScript(Entity* owner)
		: Component(owner) {}

	bool Initialize(Entity* owner, std::string scriptFile);

	void Start() override;
	void Update(const float& deltaTime) override;
	void Destroy() override;
	void OnImGuiRender() override;

	void InitFromJSON(Entity* owner, const rapidjson::Value& componentInformation) override;
	void WriteToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;

	bool lua_KeyIsPressed(int keycode);

	std::string& GetFilePath() { return filePath; }


private:

	LuaPlus::LuaState* luaState = nullptr;
	LuaPlus::LuaObject Lua_UpdateFunction;
	std::string filePath;

	float m_callDelay = 0.01f;
	const float MAX_CALL_DELAY = 0.01f;
};
