#pragma once

#include "Insight/Runtime/Game_Framework/Game_Core.h"

class GAME_API InsightGame : public Insight::IGame
{
public:
	InsightGame() {}
	virtual ~InsightGame() {}

	virtual void OnAppUpdate() override
	{

	}

	virtual Insight::ModuleLoadStatus GetLoadStatus() { return Insight::ModuleLoadStatus::Success; }

};

GAME_API void* CreateGameInstance()
{
	return static_cast<void*>(new InsightGame());
}
