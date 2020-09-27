#pragma once
#include <Retina/Core.h>

#include "Retina/Core/Layer/Layer.h"

#include "Retina/Events/Mouse_Event.h"

#include "Retina/Physics/Ray.h"
#include "Retina/Math/ie_Vectors.h"

namespace Retina {

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

	class RETINA_API EditorLayer : public Layer
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
		// TEMP
		ieVector3 GetMouseDirectionVector();
		bool hit_sphere(const ieVector3& center, float radius, const Physics::Ray& r);

		bool DispatchObjectSelectionRay(MouseButtonPressedEvent& e);
	private:
		Runtime::AActor*		m_pSelectedActor = nullptr;
		SceneNode*				m_pSceneRootRef = nullptr;
		Runtime::ACamera*		m_pSceneCameraRef = nullptr;
		Scene*					m_pCurrentSceneRef = nullptr;
		ImGuiLayer*				m_pImGuiLayerRef = nullptr;

		bool					m_UIEnabled = true;
	};

}