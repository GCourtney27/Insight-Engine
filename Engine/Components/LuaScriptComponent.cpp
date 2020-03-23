#include "LuaScriptComponent.h"
#include "..\Editor\Editor.h"
#include "..\Graphics\ImGui\imgui.h"
#include <codecvt>
#include <locale>

#include "RigidBodyComponent.h"


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

	m_pLuaState = LuaPlus::LuaState::Create();
	//this->m_pLuaState = LuaStateManager::GetStateManager()->GetLuaState();

	// -- Set Lua Callable Functions -- //
	// Debugging
	m_pLuaState->GetGlobals().RegisterDirect("DebugLog", (*this), &LuaScript::lua_DebugLog);
	// Input
	m_pLuaState->GetGlobals().RegisterDirect("OnKeyPressed", (*this), &LuaScript::lua_KeyIsPressed);
	m_pLuaState->GetGlobals().RegisterDirect("OnKeyReleased", (*this), &LuaScript::lua_KeyIsReleased);
	m_pLuaState->GetGlobals().RegisterDirect("GetMouseHorizontal", (*this), &LuaScript::GetAxisHorizontal);
	m_pLuaState->GetGlobals().RegisterDirect("GetMouseVertical", (*this), &LuaScript::GetAxisVertical);
	m_pLuaState->GetGlobals().RegisterDirect("MouseMoved", (*this), &LuaScript::MouseMoved);
	// Transforms
	m_pLuaState->GetGlobals().RegisterDirect("AdjustRotation", (*m_owner), &Entity::lua_AdjustRotation);
	m_pLuaState->GetGlobals().RegisterDirect("AdjustPosition", (*m_owner), &Entity::lua_AdjustPosition);
	m_pLuaState->GetGlobals().RegisterDirect("AdjustScale", (*m_owner), &Entity::lua_AdjustScale);
	m_pLuaState->GetGlobals().RegisterDirect("GetPositionX", (*m_owner), &Entity::lua_GetPosX);
	m_pLuaState->GetGlobals().RegisterDirect("GetPositionY", (*m_owner), &Entity::lua_GetPosY);
	m_pLuaState->GetGlobals().RegisterDirect("GetPositionZ", (*m_owner), &Entity::lua_GetPosZ);

	// Physics
	m_pLuaState->GetGlobals().RegisterDirect("Translate", (*this), &LuaScript::lua_Translate);
	// Engine
	m_pLuaState->GetGlobals().RegisterDirect("Instantiate", (*this), &LuaScript::lua_Instantiate);


	return true;
}

bool LuaScript::lua_IsColliding()
{
	return m_owner->PhysicsIsColliding();
}

void LuaScript::lua_Translate(float x, float y, float z)
{

	RigidBody* rb = m_owner->GetComponent<RigidBody>();
	if (rb != nullptr)
	{
		rb->Translate(x, y, z);
		return;
	}
	m_owner->GetTransform().AdjustPosition(x, y, z);
}

void LuaScript::lua_DebugLog(const char * message)
{
	std::string msg = "[" + StringHelper::GetFilenameFromDirectory(this->filePath) + "] ";
	msg += message;
	Debug::Editor::Instance()->DebugLog(msg);
}

void LuaScript::lua_Instantiate(const char * instanceType)
{
 	if (std::string(instanceType) == "Entity")
		Entity::CreateEntityWithDefaultParams();
}

bool LuaScript::MouseMoved()
{
	return InputManager::Instance()->GetMouseMoved();
}

float LuaScript::GetAxisHorizontal()
{
	return InputManager::Instance()->GetMouseX();
}

float LuaScript::GetAxisVertical()
{
	return InputManager::Instance()->GetMouseY();
}

void LuaScript::Start()
{
	if (this->m_owner->HasLuaScript())
		m_pLuaState->DoFile(filePath.c_str());
	else
		return;

	LuaPlus::LuaFunctionVoid Lua_Start(m_pLuaState->GetGlobal("Start"));
	Lua_Start();

}

void LuaScript::Update(const float& deltaTime)
{
	if (!this->m_owner->HasLuaScript())
		return;

	if (!this->GetIsComponentEnabled())
		return;

	LuaPlus::LuaObject globals = m_pLuaState->GetGlobals();

	LuaPlus::LuaObject global_Update = globals.GetByName("Update");
	if (global_Update.IsFunction())
	{
		LuaPlus::LuaFunctionVoid Lua_Update(global_Update);
		Lua_Update(deltaTime);
	}
	else
	{
		DEBUGLOG("Failed to locate Update() in Lua script: " + this->filePath)
	}

	if (lua_IsColliding())
	{
		LuaPlus::LuaObject global_ColEnter = globals.GetByName("OnCollisionEnter");
		if (global_ColEnter.IsFunction())
		{
			LuaPlus::LuaFunctionVoid Lua_OnCollisionEnter(global_ColEnter);
			// Pass in the colliding objects name
			std::string collidingObj = m_owner->GetComponent<RigidBody>()->GetCollidingObjectName();
			Lua_OnCollisionEnter(collidingObj.c_str());
		}
		else
		{
			DEBUGLOG("Failed to locate OnCollisionEnter() in Lua script: " + this->filePath)
		}

	}

	/*
	m_callDelay -= deltaTime;
	if (m_callDelay < 0.0f)
	{
		

		m_callDelay = MAX_CALL_DELAY;
	}
	*/

}

bool LuaScript::lua_KeyIsPressed(int keycode)// This was an int before, and it was passed right into the KeyIsPressed func
{
	return InputManager::Instance()->keyboard.KeyIsPressed(keycode);
}

bool LuaScript::lua_KeyIsReleased(int keycode)
{
	//return InputManager::Instance()->keyboard.
	return false;
}

void LuaScript::Destroy()
{
	m_pLuaState->Destroy(m_pLuaState);
}

void LuaScript::OnImGuiRender()
{
	ImGui::Text(GetName());

	static bool isLSEnabled = this->GetIsComponentEnabled();
	if (ImGui::Checkbox("Is Script Enabled", &isLSEnabled))
	{
		this->SetComponentEnabled(isLSEnabled);
	}
}

void LuaScript::OnEditorStop()
{
}

void LuaScript::ConvertFloat3ToTable(const DirectX::XMFLOAT3 & vector, LuaPlus::LuaObject & outLuaTable) const
{
	outLuaTable.AssignNewTable(m_pLuaState);
	outLuaTable.SetNumber("x", vector.x);
	outLuaTable.SetNumber("y", vector.y);
	outLuaTable.SetNumber("z", vector.z);
}

void LuaScript::ConvertTableToFloat3(const LuaPlus::LuaObject & luaTable, DirectX::XMFLOAT3& outVector) const
{
	LuaPlus::LuaObject temp;

	// x
	temp = luaTable.Get("x");
	if (temp.IsNumber())
		outVector.x = temp.GetFloat();
	else
		ErrorLogger::Log("luaTable.x is not a number");

	// y
	temp = luaTable.Get("y");
	if (temp.IsNumber())
		outVector.y = temp.GetFloat();
	else
		ErrorLogger::Log("luaTable.y is not a number");

	// z
	temp = luaTable.Get("z");
	if (temp.IsNumber())
		outVector.z = temp.GetFloat();
	else
		ErrorLogger::Log("luaTable.z is not a number");

}

