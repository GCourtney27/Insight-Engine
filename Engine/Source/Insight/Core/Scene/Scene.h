#pragma once

#include <Insight/Core.h>

#include "Scene_Node.h"
#include "Insight/Rendering/Rendering_Context.h"
#include "Insight/Systems/Managers/Resource_Manager.h"
#include "Insight/Systems/File_System.h"
#include "Insight/Runtime/ACamera.h"


namespace Insight {


	class APlayerCharacter;
	class APlayerStart;

	class INSIGHT_API Scene
	{
	public:
		Scene();
		~Scene();

		SceneNode* GetRootNode() const { return m_pSceneRoot; }
		bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

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
		bool FlushAndOpenNewScene(const std::string& NewScene);

		SceneNode& GetSceneRoot() { return *m_pSceneRoot; }
		ACamera& GetSceneCamera() { return *m_pCamera; }

		void SetDisplayName(const std::string& name) { m_DisplayName = name; }
		std::string GetDisplayName() { return m_DisplayName; }
		void EndPlaySession();

	private:

	private:
		APlayerCharacter* m_pPlayerCharacter = nullptr;
		APlayerStart* m_pPlayerStart = nullptr;
		ACamera* m_pCamera = nullptr;
		ViewTarget m_EditorViewTarget;

		ieVector3 newPos;
		AActor* m_pSelectedActor = nullptr;

		SceneNode* m_pSceneRoot = nullptr;
		std::shared_ptr<RenderingContext> m_Renderer = nullptr;
		std::string m_DisplayName;
		
	private:
		ResourceManager m_ResourceManager;

	};

}