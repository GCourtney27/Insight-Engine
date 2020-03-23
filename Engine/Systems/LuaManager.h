#pragma once

#include <vector>

class LuaScript;

class LuaManager
{
public:
	LuaManager() {}
	~LuaManager() {}

	void Flush();

	void AddScript(LuaScript* script) { m_scripts.push_back(script); }

private:
	std::vector<LuaScript*> m_scripts;

};