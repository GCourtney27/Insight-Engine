#pragma once
#include "..\\Input\\InputManager.h"
#include "..\\Physics\\Ray.h"
#include "..\\Objects\\Entity.h"
#include "..\\Engine.h"

namespace Debug
{
	class Editor : public Singleton<Editor>
	{
	public:
		Editor() {}

		bool Initialize(Engine* engine);
		void Update();
		void Shutdown();
		//void ImGuiRender();

		DirectX::XMFLOAT3 GetMouseDirectionVector();
		bool hit_sphere(const SimpleMath::Vector3& center, float radius, const Ray& r);
		float intersection_distance(const SimpleMath::Vector3& center, float radius, const Ray& r);

		bool SaveScene();

		bool PlayingGame() { return m_playingGame; }
		void PlayGame() { m_playingGame = true; }
		void StopGame() { m_playingGame = false; }

		void SetIsEditorEnabled(bool enabled) { m_isEditorEnabled = enabled; }
		bool IsEditorEnabled() { return m_isEditorEnabled; }

		Entity* GetSelectedEntity() { return m_selectedEntity; }

	private:
		Engine* m_engine;

		Entity* m_selectedEntity = nullptr;
		bool m_isEditorEnabled = true;
		bool m_playingGame = false;
	};
}
