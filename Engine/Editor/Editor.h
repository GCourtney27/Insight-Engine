#pragma once
#include "..\\Input\\InputManager.h"
#include "..\\Physics\\Ray.h"
#include "..\\Objects\\Entity.h"
#include "..\\Engine.h"

// ImGui
#include "..\Graphics\ImGui\imgui.h"
#include "..\Graphics\ImGui\imgui_impl_win32.h"
#include "..\Graphics\ImGui\imgui_impl_dx11.h"
#include "..\Graphics\ImGui\ImGuizmo.h"

#define DEBUGLOG(x) Debug::Editor::Instance()->DebugLog(x);

namespace Debug
{
	class Editor : public Singleton<Editor>
	{
	public:
		Editor() {}

		bool Initialize(Engine* engine, HWND hwnd);
		void Update(float deltaTime);
		void Shutdown();

		std::string GetLogStatement() { return m_debugLog; }
		void DebugLog(std::string log) { m_debugLog += log + "\n"; }
		void DebugLogChar(const char* log) { m_debugLog += log; }
		void ClearConsole() { m_debugLog = ""; }

		DirectX::XMFLOAT3 GetMouseDirectionVector();
		bool hit_sphere(const SimpleMath::Vector3& center, float radius, const Ray& r);
		float intersection_distance(const SimpleMath::Vector3& center, float radius, const Ray& r);

		bool SaveScene();

		bool PlayingGame() { return m_playingGame; }
		void PlayGame() 
		{
			m_playingGame = true; 
			if (m_clearConsoleOnPlay && !m_playingGame) ClearConsole();
			m_pEngine->OnGameStart();
		}
		void StopGame() 
		{
			m_playingGame = false;
			std::list<Entity*>* entities = m_pEngine->GetScene().GetAllEntities();
			std::list<Entity*>::iterator iter;
			for (iter = entities->begin(); iter != entities->end(); iter++)
			{
				(*iter)->OnEditorStop();
			}
		}

		void SetIsEditorEnabled(bool enabled) { m_isEditorEnabled = enabled; }
		bool IsEditorEnabled() { return m_isEditorEnabled; }

		Entity* GetSelectedEntity() { return m_pSelectedEntity; }
		void SetSelectedEntity(Entity* entity) { m_pSelectedEntity = entity; }

		bool& GetClearConsoleOnPlay() { return m_clearConsoleOnPlay; }
		void SetCLearConsoleOnPlay(bool cop) { m_clearConsoleOnPlay = cop; }

	private:
		Engine* m_pEngine;
		Entity* m_pSelectedEntity = nullptr;
		ImGuiIO* m_pImGuiIO = nullptr;

		bool m_isEditorEnabled = true;
		bool m_playingGame = false;

		bool m_clearConsoleOnPlay = false;

		std::string m_debugLog;
	};
}
