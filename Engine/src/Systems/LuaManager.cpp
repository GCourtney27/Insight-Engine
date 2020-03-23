#include "LuaManager.h"

#include "../Components/LuaScriptComponent.h"

void LuaManager::Flush()
{
	m_scripts.clear();
}
