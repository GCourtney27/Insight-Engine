#include "LuaScriptComponent.h"
#include "..\Editor\Editor.h"
#include "..\Graphics\ImGui\imgui.h"

void LuaScript::InitFromJSON(Entity* owner, const rapidjson::Value& componentInformation)
{
	std::string scriptFilePath;

	for (rapidjson::SizeType l = 0; l < componentInformation.Size(); l++)
	{
		json::get_string(componentInformation[l], "FilePath", scriptFilePath);
	}

	this->Initialize(owner, scriptFilePath);

}

void LuaScript::WriteToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.Key("LuaScript");
	writer.StartArray(); // Start Lua Script

	writer.StartObject(); // Start FilePath
	writer.Key("FilePath");
	writer.String(this->filePath.c_str());
	writer.EndObject(); // End FilePath

	writer.EndArray(); // End Lua Script
}

bool LuaScript::Initialize(Entity* owner, std::string scriptFile)
{
	this->m_owner = owner;
	filePath = scriptFile;
	SetName("Lua Script");

	if (scriptFile == "NONE")
		owner->SetHasLuaScript(false);

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

void LuaScript::Start()
{
	if(this->m_owner->HasLuaScript())
		luaState->DoFile(filePath.c_str());

}

void LuaScript::Update(const float& deltaTime)
{
	if (!this->m_owner->HasLuaScript())
		return;

	m_callDelay -= deltaTime;
	if (m_callDelay < 0.0f)
	{
		LuaPlus::LuaFunctionVoid Lua_Update(luaState->GetGlobal("Update"));
		Lua_Update(deltaTime);

		m_callDelay = MAX_CALL_DELAY;
	}

}

bool LuaScript::lua_KeyIsPressed(int keycode)// This was an int before, and it was passed right into the KeyIsPressed func
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

