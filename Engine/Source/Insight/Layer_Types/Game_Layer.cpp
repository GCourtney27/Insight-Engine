#include <Engine_pch.h>

#include "Game_Layer.h"

#include "Insight/Core/Scene/Scene.h"

namespace Insight {
	
	
	GameLayer::GameLayer()
		: Layer("Game Layer")
	{
		m_pScene = new Scene();
	}

	GameLayer::~GameLayer()
	{
	}

	void GameLayer::BeginPlay()
	{
		m_TickScene = true;
		ResourceManager::Get().GetMonoScriptManager().OnBeginPlay();
		m_pScene->BeginPlay();
	}

	void GameLayer::Update(const float DeltaMs)
	{
		m_pScene->OnUpdate(DeltaMs);
	}

	void GameLayer::PreRender()
	{
		m_pScene->OnPreRender();
	}

	void GameLayer::Render()
	{
		m_pScene->OnRender();
		m_pScene->OnMidFrameRender();
	}

	void GameLayer::EndPlay()
	{
		m_TickScene = false;
		m_pScene->EndPlaySession();
		ResourceManager::Get().GetMonoScriptManager().OnEndPlaySession();
	}

	void GameLayer::PostInit()
	{
		m_pScene->PostInit();
	}
	void GameLayer::OnAttach()
	{
		IE_CORE_INFO("Game Layer Attached");
		BeginPlay();
	}

	void GameLayer::OnDetach()
	{
		IE_CORE_INFO("Game Layer Detached");
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
			IE_CORE_ERROR("Failed to load Scene \"{0}\"", FileName);
			return false;
		}
		return true;
	}
	
}

