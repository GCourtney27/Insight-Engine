// Copyright Insight Interactive. All Rights Reserved.
#pragma once
#include "EngineDefines.h"

#include "Core/Public/Layer/Layer.h"

#include "Core/Public/Events/MouseEvent.h"

#include "Physics/Ray.h"

namespace Insight {

	class Scene;
	class SceneNode;
	class ImGuiLayer;

	namespace GameFramework {
		class ACamera;
		class AActor;
	}

	class ASpotLight;
	class APointLight;
	class ADirectionalLight;

	class INSIGHT_API EditorLayer : public Layer
	{
	public:
		EditorLayer();
		~EditorLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		virtual void OnUpdate(const float DeltaMs) override;
		void OnEvent(Event& event) override;

		inline void SetUIEnabled(bool Enabled) { m_UIEnabled = Enabled; }
		void SetSelectedActor(GameFramework::AActor* actor) { m_pSelectedActor = actor; }
		GameFramework::AActor* GetSelectedActor() { return m_pSelectedActor; }

	private:
		void RenderSceneHeirarchy();
		void RenderInspector();
		void RenderSelectionGizmo();
		void RenderCreatorWindow();
		void RenderRendererSettings();

	private:
		GameFramework::AActor*		m_pSelectedActor = nullptr;
		SceneNode*				m_pSceneRootRef = nullptr;
		GameFramework::ACamera*		m_pSceneCameraRef = nullptr;
		Scene*					m_pCurrentSceneRef = nullptr;
		ImGuiLayer*				m_pImGuiLayerRef = nullptr;

		bool					m_UIEnabled = true;
	};

}