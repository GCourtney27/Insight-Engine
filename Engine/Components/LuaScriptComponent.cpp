#include "LuaScriptComponent.h"
#include "..\Editor\Editor.h"
#include "..\Systems\Timer.h"
#include "..\Graphics\ImGui\imgui.h"

void LuaScript::InitFromJSON(Entity* owner, const rapidjson::Value& componentInformation)
{
	std::string scriptFilePath;

	for (rapidjson::SizeType l = 0; l < componentInformation.Size(); l++)
	{
		json::get_string(componentInformation[l], "FilePath", scriptFilePath);
	}

	this->Initialize(owner, scriptFilePath);

	/*LuaScript* finalScript = nullptr;
	std::string scriptFilePath;
	for (rapidjson::SizeType l = 0; l < luaScript.Size(); l++)
	{
		json::get_string(luaScript[l], "FilePath", scriptFilePath);
		if (scriptFilePath != "NONE")
			finalScript = entity->AddComponent<LuaScript>();
		else
			break;
	}
	if (finalScript != nullptr)
		finalScript->Initialize(entity, scriptFilePath);*/
}

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


	return true;
}

void LuaScript::Update(float deltaTime)
{
	
	luaState->DoFile(filePath.c_str());

	LuaPlus::LuaFunctionVoid LUpdate(luaState->GetGlobal("Update"));
	LUpdate(deltaTime);


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

