#include "LuaScriptComponent.h"
#include "..\Editor\Editor.h"

bool LuaScript::Initialize(std::string scriptFile)
{
	L = luaL_newstate();
	cmd = "a = 7 + 11";
	filePath = scriptFile;
	return true;
}

void LuaScript::Update()
{
	callCounter--;
	if (callCounter >= 0)
	{
		int r = luaL_dofile(L, filePath.c_str());
		//int r = luaL_dostring(L, cmd.c_str());
		if (r == LUA_OK)
		{
			lua_getglobal(L, "a");
			if (lua_isnumber(L, -1))
			{
				int num = (int)lua_tonumber(L, -1);
				std::string result_str = "result: " + std::to_string(num);
				Debug::Editor::Instance()->DebugLog(result_str);
			}
		}
		else
		{
			std::string errorMsg = lua_tostring(L, -1);
			Debug::Editor::Instance()->DebugLog(errorMsg);

		}
	}
	
}

void LuaScript::Destroy()
{

}

void LuaScript::OnImGuiRender()
{

}