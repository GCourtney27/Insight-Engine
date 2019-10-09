#include "ScriptExports.h"
#include "LuaStateManager.h"
#include "..\Editor\Editor.h"

class InternalScriptExports
{
public:
	// Initialization
	static bool Initialize(void);
	static void Destroy(void);

	// Resource loading
	static bool LoadAndExecuteScriptResource(const char* scriptResource);

	// Create entity
	//static void CreateEntity
};

bool InternalScriptExports::Initialize(void)
{
	return true;
}

void InternalScriptExports::Destroy(void)
{
}

bool InternalScriptExports::LoadAndExecuteScriptResource(const char * scriptResource)
{
	return false;
}

void ScriptExports::Register(void)
{
	LuaPlus::LuaObject globals = LuaStateManager::GetStateManager()->GetGlobalVars();

	// Initialize
	InternalScriptExports::Initialize();

	// Resource loading
	globals.RegisterDirect("LoadAndExecuteScriptResource", &InternalScriptExports::LoadAndExecuteScriptResource);

	// Debug
	//globals.RegisterDirect("DebugLog", &Debug::Editor::Instance()->DebugLog);

}

void ScriptExports::Unregister(void)
{
	InternalScriptExports::Destroy();
}