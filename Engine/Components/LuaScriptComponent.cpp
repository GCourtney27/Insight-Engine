#include "LuaScriptComponent.h"
#include "..\Editor\Editor.h"
#include "..\Systems\Timer.h"
#include "..\Graphics\ImGui\imgui.h"

bool LuaScript::Initialize(Entity* owner, std::string scriptFile)
{
	this->m_owner = owner;
	filePath = scriptFile;
	SetName("Lua Script");

	luaState = LuaPlus::LuaState::Create();

	// Set Lua Function Calls
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


}

bool LuaScript::lua_KeyIsPressed(int  keycode)// This was an int before, and it was passed right into the KeyIsPressed func
{
	return InputManager::Instance()->keyboard.KeyIsPressed(keycode);
}


void LuaScript::Destroy()
{

}

void LuaScript::OnImGuiRender()
{
	ImGui::Text(GetName());

}

void LuaScript::JSONLoad()
{
}
