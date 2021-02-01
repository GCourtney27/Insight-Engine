#include <Engine_pch.h>

#include "GameLayer.h"

#include "Runtime/Core/Scene/Scene.h"

namespace Insight {
	
	
	GameLayer::GameLayer()
		:	Layer("Game Layer"),
			m_TickScene(false)
	{
		m_pScene = new Scene();
	}

	GameLayer::~GameLayer()
	{
		delete m_pScene;
	}

	void GameLayer::BeginPlay()
	{
		m_TickScene = true;
#if defined (IE_PLATFORM_BUILD_WIN32)
		ResourceManager::Get().GetMonoScriptManager().OnBeginPlay();
#endif
		m_pScene->BeginPlay();
	}

	void GameLayer::Update(const float DeltaMs)
	{
		m_pScene->OnUpdate(DeltaMs);
	}

	void GameLayer::EndPlay()
	{
		m_TickScene = false;
		m_pScene->EndPlaySession();
#if defined (IE_PLATFORM_BUILD_WIN32)
		ResourceManager::Get().GetMonoScriptManager().OnEndPlaySession();
#endif
	}

	void GameLayer::PostInit()
	{
		m_pScene->PostInit();
	}
	void GameLayer::OnAttach()
	{
		IE_DEBUG_LOG(LogSeverity::Log, "Game Layer Attached");
		BeginPlay();
	}

	void GameLayer::OnDetach()
	{
		IE_DEBUG_LOG(LogSeverity::Log, "Game Layer Detached");
		EndPlay();
	}

	void GameLayer::OnImGuiRender()
	{

	}

	void GameLayer::OnUpdate(const float DeltaMs)
	{
		m_pScene->Tick(DeltaMs);
	}

	void GameLayer::OnEvent(Event& event)
	{

	}

	bool GameLayer::LoadScene(const std::string& FileName)
	{
		if (!m_pScene->Init(FileName)) {
			IE_DEBUG_LOG(LogSeverity::Error, "Failed to load Scene \"{0}\"", FileName);
			return false;
		}
		return true;
	}
	
}

