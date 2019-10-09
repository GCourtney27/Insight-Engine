#pragma once

class IScriptManager
{
public:
	virtual ~IScriptManager() {}
	virtual bool VInitialize(void) = 0;
	virtual void VExecuteFile(const char* resource) = 0;
	virtual void VExecuteString(const char * str) = 0;
};