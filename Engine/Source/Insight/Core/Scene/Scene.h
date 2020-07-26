#pragma once

#include <Insight/Core.h>

#include "Scene_Node.h"
#include "Insight/Rendering/Renderer.h"
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

		// Get the scene root of the application that all 
		// other game actors in the world are children too.
		SceneNode* GetRootNode() const { return m_pSceneRoot; }
		// Write scene out to JSON file.
		bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

		// Initialize the scene
		bool Init(const std::string fileName);
		// Post-Initialize of the scene. Used for dispatching 
		// initialization commands for actors on the GPU (Mesh 
		// creation, texture generation etc..)
		bool PostInit();
		// Called when the world begins a play session. Or a new Scene 
		// is loaded during game runtime 
		void BeginPlay();
		// Called once per frame during game runtime. 
		void Tick(const float& deltaMs);
		// Called once per frame during the application's runtime. Some resoruces
		// aways need to be updated, such as pushing geometry to the GPU for example. This
		// guarantees it get exexcuted regardless if the game is simmulating or not.
		void OnUpdate(const float& deltaMs);
		// Render an ImGui widget for thie module.
		void OnImGuiRender();
		// Flushes GPU command lists and prepares for next frame. Also calculates 
		// the parent child relationships of all actors in the scene. Once calcualted,
		// the constant buffers are updated on the GPU.
		void OnPreRender();
		// Begins the geometry pass on the scene. Anything that is not transparent or you would like to be 
		// included in PBR lighting must be drawn between now and 'OnMidFrameRender'.
		void OnRender();
		// Ends the geometry pass and begins the lighting pass.
		void OnMidFrameRender();
		// Ends the light pass begins the post-process pass. Once complete, swaps buffers and 
		// presents the final frame
		void OnPostRender();
		// Destroys the scene and releases resources. DOES NOT save scene.
		void Destroy();
		// Close the current scene and open a new scene.
		bool FlushAndOpenNewScene(const std::string& NewScene);

		SceneNode& GetSceneRoot() { return *m_pSceneRoot; }
		ACamera& GetSceneCamera() { return *m_pCamera; }

		// Set the name of the level.
		void SetDisplayName(const std::string& name) { m_DisplayName = name; }
		// Gets the name of the level.
		std::string GetDisplayName() { return m_DisplayName; }
		// Ends the game runtime simulation for the editor.
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
		std::string m_DisplayName;
		
	private:
		ResourceManager m_ResourceManager;

	};

}