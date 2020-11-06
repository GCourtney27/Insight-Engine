// Copyright Insight Interactive. All Rights Reserved.
#pragma once
#include <Insight/Core.h>

#include "Insight/Core/Layer/Layer.h"

#include "Insight/Events/Mouse_Event.h"

#include "Insight/Physics/Ray.h"
#include "Insight/Math/ie_Vectors.h"

namespace Insight {

	class Scene;
	class SceneNode;
	class ImGuiLayer;

	namespace Runtime {
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
		void SetSelectedActor(Runtime::AActor* actor) { m_pSelectedActor = actor; }
		Runtime::AActor* GetSelectedActor() { return m_pSelectedActor; }

	private:
		void RenderSceneHeirarchy();
		void RenderInspector();
		void RenderSelectionGizmo();
		void RenderCreatorWindow();
	private:
		Runtime::AActor*		m_pSelectedActor = nullptr;
		SceneNode*				m_pSceneRootRef = nullptr;
		Runtime::ACamera*		m_pSceneCameraRef = nullptr;
		Scene*					m_pCurrentSceneRef = nullptr;
		ImGuiLayer*				m_pImGuiLayerRef = nullptr;

		bool					m_UIEnabled = true;
	};

}