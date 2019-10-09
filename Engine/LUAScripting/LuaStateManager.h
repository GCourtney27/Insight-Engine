#pragma once

#include "Lua/LuaPlus/LuaPlus.h"
#include "IScriptManager.h"
#include <string>
#include <DirectXMath.h>

class LuaStateManager : public IScriptManager
{
	static LuaStateManager* s_pSingleton;
	LuaPlus::LuaState* m_pLuaState;
	std::string m_lastError;

public:
	// Singleton functions
	static bool Create(void);
	static void Destroy(void);
	static LuaStateManager* GetStateManager(void) { return s_pSingleton; }

	// IScriptManager interafce
	virtual bool VInitialize() override;
	virtual void VExecuteFile(const char* resource) override;
	virtual void VExecuteString(const char* str) override;

	LuaPlus::LuaObject GetGlobalVars(void);
	LuaPlus::LuaState* GetLuaState(void) const;

	// Public helpers
	LuaPlus::LuaObject CreatePath(const char* pathString, bool toIgnorelastElement = false);
	void ConvertFloat3ToTable(const DirectX::XMFLOAT3 & vector, LuaPlus::LuaObject & outLuaTable) const;
	void ConvertTableToFloat3(const LuaPlus::LuaObject & luaTable, DirectX::XMFLOAT3 & outVector) const;

private:
	void SetError(int errorNum);
	void ClearStack(void);

	//Private constructor and destructor; call the static Create() and Destroy() instead
	explicit LuaStateManager(void);
	virtual ~LuaStateManager(void);
};