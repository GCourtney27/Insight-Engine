#pragma once
//#include "..\LUAScripting\LuaStateManager.h"
#include "Component.h"
#include "..\Framework\Singleton.h"
#include "Lua/LuaPlus/LuaPlus.h"
#include <DirectXMath.h>

extern "C"
{
	#include "Lua535/lua.h"
	#include "Lua535/lauxlib.h"
	#include "Lua535/lualib.h"
	#include "Lua535/luaconf.h"
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
	void OnEditorStop() override;

	void InitFromJSON(Entity* owner, const rapidjson::Value& componentInformation) override;
	void WriteToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;

	bool MouseMoved();
	float GetAxisHorizontal();
	float GetAxisVertical();

	bool lua_KeyIsPressed(int keycode);
	bool lua_KeyIsReleased(int keycode);
	bool lua_IsColliding();
	void lua_Translate(float x, float y, float z);
	void lua_DebugLog(const char* message);
	void lua_Instantiate(const char* instanceType);

	void ConvertFloat3ToTable(const DirectX::XMFLOAT3 & vector, LuaPlus::LuaObject & outLuaTable) const;
	void ConvertTableToFloat3(const LuaPlus::LuaObject & luaTable, DirectX::XMFLOAT3 & outVector) const;

	std::string& GetFilePath() { return filePath; }


private:

	LuaPlus::LuaState* m_pLuaState = nullptr;
	LuaPlus::LuaObject Lua_UpdateFunction;
	std::string filePath;

	float m_callDelay = 0.01f;
	const float MAX_CALL_DELAY = 0.01f;
};
