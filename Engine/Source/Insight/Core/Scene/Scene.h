#pragma once

#include <Insight/Core.h>

#include "Scene_Node.h"
#include "Insight/Rendering/Rendering_Context.h"
#include "Insight/Runtime/APlayer_Character.h"
#include "Insight/Rendering/Lighting/APoint_Light.h"
#include "Insight/Rendering/Geometry/Model.h"
#include "Insight/Systems/Model_Manager.h"
#include "Insight/Systems/File_System.h"

namespace Insight {


	class INSIGHT_API Scene
	{
	public:
		Scene();
		~Scene();

		SceneNode* GetRootNode() const { return m_pSceneRoot; }

		bool Init();
		bool PostInit();
		void BeginPlay();
		void Tick(const float& deltaMs);
		void OnUpdate(const float& deltaMs);
		void OnImGuiRender();
		void OnPreRender();
		void OnRender();
		void OnMidFrameRender();
		void OnPostRender();
		void Destroy();
		void Flush();

		// Editor
		void SetSelectedActor(AActor* actor) { m_pSelectedActor = actor; }

	private:
		void RenderSceneHeirarchy();
		void RenderInspector();
		void RenderCreatorWindow();
	private:
		AActor* m_pTestActor = nullptr; // Temp: this should come from a file
		AActor* m_pTestActor2 = nullptr; // Temp: this should come from a file
		AActor* m_pTestActor3 = nullptr; // Temp: this should come from a file
		APointLight* m_pTestPointLight = nullptr;
		APointLight* m_pTestPointLight1 = nullptr;
		APlayerCharacter* m_pPlayerCharacter;

		AActor* m_pSelectedActor = nullptr;

		FileSystem m_FileSystem;
		ModelManager m_ModelManager;
		SceneNode* m_pSceneRoot = nullptr;
		std::shared_ptr<RenderingContext> m_Renderer = nullptr;
	};

}