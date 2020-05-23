#pragma once

#include <Insight/Core.h>

#include "Scene_Node.h"
#include "Insight/Rendering/Rendering_Context.h"
#include "Insight/Runtime/APlayer_Character.h"
#include "Insight/Rendering/Geometry/Model.h"
#include "Insight/Systems/Model_Manager.h"

namespace Insight {


	class INSIGHT_API Scene
	{
	public:
		Scene();
		~Scene();

		SceneNode* GetRootNode() const { return m_pRoot; }

		bool Init();
		bool PostInit();
		void BeginPlay();
		void Tick(const float& deltaMs);
		void OnUpdate(const float& deltaMs);
		void OnImGuiRender();
		void OnPreRender();
		void OnRender();
		void OnPostRender();
		void Destroy();
		void Flush();

	private:
		void RenderSceneHeirarchy();
		void RenderInspector();
	private:
		AActor* m_pTestActor = nullptr; // Temp: this should come from a file
		AActor* m_pTestActor2 = nullptr; // Temp: this should come from a file
		Model* m_pModel = nullptr; // TEMP!!

		//ModelManager m_ModelManager;
		SceneNode* m_pRoot = nullptr;
		std::shared_ptr<RenderingContext> m_Renderer = nullptr;
	};

}