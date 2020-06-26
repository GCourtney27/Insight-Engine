#pragma once

#include <Insight/Core.h>

#include "Scene_Node.h"
#include "Insight/Rendering/Rendering_Context.h"
#include "Insight/Systems/Managers/Resource_Manager.h"
#include "Insight/Systems/File_System.h"
#include "Insight/Runtime/ACamera.h"


class APlayerCharacter;

namespace Insight {

	class INSIGHT_API Scene
	{
	public:
		Scene();
		~Scene();

		SceneNode* GetRootNode() const { return m_pSceneRoot; }
		bool LoadFromJson(const std::string& fileName);

		bool Init(const std::string fileName);
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
		AActor* GetSelectedActor() { return m_pSelectedActor; }
		void SetDisplayName(const std::string& name) { m_DisplayName = name; }
		void EndPlaySession();
		bool IsPlaySesionUnderWay() { return m_TickScene; }

	private:
		void RenderSceneHeirarchy();
		void RenderInspector();
		void RenderCreatorWindow();
		void RenderPlayPanel();
	private:
		APlayerCharacter* m_pPlayerCharacter = nullptr;
		ACamera* m_pCamera = nullptr;
		ViewTarget m_EditorViewTarget;

		Vector3 newPos;
		AActor* m_pSelectedActor = nullptr;

		SceneNode* m_pSceneRoot = nullptr;
		std::shared_ptr<RenderingContext> m_Renderer = nullptr;
		std::string m_DisplayName;
		
		bool m_TickScene = false;

	private:
		// TODO move this to engine class
		FileSystem m_FileSystem;
		ResourceManager m_ResourceManager;

	};

}