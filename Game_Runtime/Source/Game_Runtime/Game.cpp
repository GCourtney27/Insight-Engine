#pragma once
#include "Game_Runtime/Game.h"

//namespace GameFactory
//{
	
//}
GAME_API void* CreateGameInstance()
{
return static_cast<void*>(new InsightGame());
}

int InsightGame::TESTGetVal()
{
	return 2;
}

Insight::ModuleLoadStatus InsightGame::GetLoadStatus()
{
	return Insight::ModuleLoadStatus::Success;
}
