#pragma once
#include <Insight/Core.h>

#include "Insight/Core/Layer/Layer.h"

namespace Insight {

	class SceneNode;
	class ACamera;
	class Scene;
	class INSIGHT_API EditorLayer : public Layer
	{
	public:
		EditorLayer();
		~EditorLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		virtual void OnUpdate(const float& DeltaMs) override;
		void OnEvent(Event& event) override;

		inline void SetUIEnabled(bool Enabled) { m_UIEnabled = Enabled; }
		void SetSelectedActor(AActor* actor) { m_pSelectedActor = actor; }
		AActor* GetSelectedActor() { return m_pSelectedActor; }

	private:
		void RenderSceneHeirarchy();
		void RenderInspector();
		void RenderSelectionGizmo();
		void RenderCreatorWindow();
	private:
		AActor*		m_pSelectedActor = nullptr;
		SceneNode*	m_pSceneRoot = nullptr;
		ACamera*	m_pSceneCamera = nullptr;
		Scene*		m_pCurrentScene = nullptr;

		bool		m_UIEnabled = true;
	};

}