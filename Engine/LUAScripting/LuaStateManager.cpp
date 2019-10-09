#include "LuaStateManager.h"
#include "..\ErrorLogger.h"
#include "..\Editor\Editor.h"

LuaStateManager* LuaStateManager::s_pSingleton = NULL;

bool LuaStateManager::Create(void)
{
	if (s_pSingleton)
	{
		ErrorLogger::Log("Overwriting LuaStateManager singleton");
		delete s_pSingleton;
		s_pSingleton = 0;
	}
	s_pSingleton = new LuaStateManager();
	if (s_pSingleton)
		return s_pSingleton->VInitialize();
	return false;
}

void LuaStateManager::Destroy()
{
	delete s_pSingleton;
	s_pSingleton = 0;
}

LuaStateManager::LuaStateManager(void)
{
	m_pLuaState = NULL;
}

LuaStateManager::~LuaStateManager(void)
{
	if (m_pLuaState)
	{
		LuaPlus::LuaState::Destroy(m_pLuaState);
		m_pLuaState = NULL;
	}
}

bool LuaStateManager::VInitialize(void)
{
	m_pLuaState = LuaPlus::LuaState::Create(true);
	if (m_pLuaState == nullptr)
	{
		ErrorLogger::Log("Failed to create Lua State.");
		return false;
	}

	// register functions
	m_pLuaState->GetGlobals().RegisterDirect("ExecuteFile", (*this), &LuaStateManager::VExecuteFile);
	m_pLuaState->GetGlobals().RegisterDirect("ExecuteString", (*this), &LuaStateManager::VExecuteString);
}

void LuaStateManager::VExecuteFile(const char* path)
{
	int result = m_pLuaState->DoFile(path);
	if (result != 0)
		SetError(result);
}

void LuaStateManager::VExecuteString(const char * chunk)
{
	int result = 0;

	// Most strings are passed straight through the Lua interpriter
	if (strlen(chunk) <= 1 || chunk[0] != '0')
	{
		result = m_pLuaState->DoString(chunk);
		if (result != 0)
			SetError(result);
	}
	// If the string starts with '=', wrap the statement in the print() function.
	else
	{
		std::string buffer("print(");
		buffer += (chunk + 1);
		buffer += ")";
		result = m_pLuaState->DoString(buffer.c_str());
		if (result != 0)
			SetError(result);
	}
}

void LuaStateManager::SetError(int errorNum)
{
	LuaPlus::LuaStackObject stackObj(m_pLuaState, -1);
	const char* errStr = stackObj.GetString();
	if (errStr)
	{
		m_lastError = errStr;
		ClearStack();
	}
	else
		m_lastError = "Unknown Lua parse error";

	ErrorLogger::Log(m_lastError);
	Debug::Editor::Instance()->DebugLog(m_lastError);
}

void LuaStateManager::ClearStack(void)
{
	m_pLuaState->SetTop(0);
}

LuaPlus::LuaObject LuaStateManager::GetGlobalVars(void)
{
	/*if (m_pLuaState == nullptr)
		ErrorLogger::Log("Attempting to return globals for null Lua state");
	else*/
		return m_pLuaState->GetGlobals();
}

LuaPlus::LuaState* LuaStateManager::GetLuaState(void) const
{
	return m_pLuaState;
}

void SplitOn(const std::string& str, std::vector<std::string>& vec, char delimiter)
{
	vec.clear();
	size_t strLen = str.size();
	if (strLen == 0)
		return;

	size_t startIndex = 0;
	size_t indexOfDel = str.find_first_of(delimiter, startIndex);
	while (indexOfDel != std::string::npos)
	{
		vec.push_back(str.substr(startIndex, indexOfDel - startIndex));
		startIndex = indexOfDel + 1;
		if (startIndex >= strLen)
			break;
		indexOfDel = str.find_first_of(delimiter, startIndex);
	}
	if (startIndex < strLen)
		vec.push_back(str.substr(startIndex));
}

LuaPlus::LuaObject LuaStateManager::CreatePath(const char * pathString, bool toIgnoreLastElement)
{
	std::vector<std::string> splitPath;
	SplitOn(pathString, splitPath, '.');
	if (toIgnoreLastElement)
		splitPath.pop_back();

	LuaPlus::LuaObject context = GetGlobalVars();
	for (auto it = splitPath.begin(); it != splitPath.end(); it++)
	{
		// Make sure we still have a valid context
		if (context.IsNil())
		{
			std::string error = "Something broke in CreatePath(); bailing ut (element == " + (*it) + ")";
			ErrorLogger::Log(error);
			return context; // This will be nil
		}

		// Grab whatever exists in the element
		const std::string & element = (*it);
		LuaPlus::LuaObject curr = context.GetByName(element.c_str());
		if (!curr.IsTable())
		{
			// If element is not a table and not nil, we clobber it
			if (!curr.IsNil())
			{
				std::string warning = "Overwriting element '" + element + "' in table";
				Debug::Editor::Instance()->DebugLog(warning);
			}
			// Element is either nil or was clobbered to add the new table
			context.CreateTable(element.c_str());
		}
		
		context = context.GetByName(element.c_str());
	}
	// If we get here we have created the path
	return context;
}

void LuaStateManager::ConvertFloat3ToTable(const DirectX::XMFLOAT3 & vector, LuaPlus::LuaObject & outLuaTable) const
{
	outLuaTable.AssignNewTable(GetLuaState());
	outLuaTable.SetNumber("x", vector.x);
	outLuaTable.SetNumber("y", vector.y);
	outLuaTable.SetNumber("z", vector.z);
}

void LuaStateManager::ConvertTableToFloat3(const LuaPlus::LuaObject & luaTable, DirectX::XMFLOAT3& outVector) const
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