#include "LuaScriptComponent.h"
#include "..\Editor\Editor.h"
#include "..\Systems\Timer.h"

bool LuaScript::Initialize(Entity* owner, std::string scriptFile)
{
	this->m_owner = owner;

	luaState = LuaPlus::LuaState::Create();

	//L = luaL_newstate();
	cmd = "a = 7 + 11";
	filePath = scriptFile;
	// Debugging
	luaState->GetGlobals().RegisterDirect("DebugLog", (*Debug::Editor::Instance()), &Debug::Editor::DebugLogChar);
	// Input
	luaState->GetGlobals().RegisterDirect("OnKeyPressed", (*this), &LuaScript::lua_KeyIsPressed);
	// Transforms
	luaState->GetGlobals().RegisterDirect("AdjustRotation", (*m_owner), &Entity::lua_AdjustRotation);
	luaState->GetGlobals().RegisterDirect("AdjustPosition", (*m_owner), &Entity::lua_AdjustPosition);
	luaState->GetGlobals().RegisterDirect("AdjustScale", (*m_owner), &Entity::lua_AdjustScale);

	

	//lua_register(L, "DebugLog", lua_PrintStringToConsole);
	//lua_register(L, "AdjustRotation", lua_AdjustRotation);

	return true;
}

void LuaScript::Update()
{
	
	luaState->DoFile(filePath.c_str());

	LuaPlus::LuaFunctionVoid LUpdate(luaState->GetGlobal("Update"));
	LUpdate(Timer::Instance()->GetDeltaTime());


#pragma region 
	/*
	if (callCounter >= 0)
	{
		callCounter--;
		int r = luaL_dofile(L, filePath.c_str());
		//int r = luaL_dostring(L, cmd.c_str());
		if (r == LUA_OK)
		{
			lua_getglobal(L, "Update");
			if (lua_isfunction(L, -1))
			{
				lua_pushnumber(L, (double)Timer::Instance()->GetDeltaTime());
				if (lua_pcall(L, 1, 0, 0))
				{

				}
			}
			//lua_getglobal(L, "a");
			/*if (lua_isnumber(L, -1))
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
	*/ 
#pragma endregion Raw Lua Calls. Probably not the best way of doing it



}

bool LuaScript::lua_KeyIsPressed(int keycode)
{
	//unsigned char key = (unsigned char)keycode;
	
	//std::string final = std::to_string(key);
	//ErrorLogger::Log(final);

	LuaPlus::LuaObject toReturn;
	if (InputManager::Instance()->keyboard.KeyIsPressed(keycode))
	{
		//ErrorLogger::Log("Pressed");
		return true;
		//luaState->PushBoolean(true);
		//toReturn.AssignBoolean(luaState, true);
	}

	return false;
}

int LuaScript::lua_PrintStringToConsole(lua_State* L)
{
	std::string luaStringToPrint = lua_tostring(L, 1);
	Debug::Editor::Instance()->DebugLog(luaStringToPrint);

	//Debug::Editor::Instance()->DebugLog("[C++] The host print has been called from engine.");

	return 0;
}


void LuaScript::Destroy()
{

}

void LuaScript::OnImGuiRender()
{

}