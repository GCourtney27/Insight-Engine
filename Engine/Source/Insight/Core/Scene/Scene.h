#pragma once

#include <Insight/Core.h>

#include "Scene_Node.h"
#include "Insight/Rendering/ASky_Sphere.h"
#include "Insight/Systems/Model_Manager.h"
#include "Insight/Rendering/Geometry/Model.h"
#include "Insight/Runtime/APlayer_Character.h"
#include "Insight/Rendering/Rendering_Context.h"
#include "Insight/Rendering/Lighting/ASpot_Light.h"
#include "Insight/Rendering/Lighting/APoint_Light.h"
#include "Insight/Rendering/Lighting/ADirectional_Light.h"
#include "Insight/Rendering/APost_Fx.h"
#include "Insight/Systems/File_System.h"

namespace Insight {

	class INSIGHT_API Scene
	{
	public:
		Scene();
		~Scene();

		SceneNode* GetRootNode() const { return m_pSceneRoot; }
		bool LoadFromJson(const std::string& fileName);

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
		void SetDisplayName(const std::string& name) { m_DisplayName = name; }

	private:
		void RenderSceneHeirarchy();
		void RenderInspector();
		void RenderCreatorWindow();
	private:
		APostFx* m_PostFxActor = nullptr;
		ASkySphere* m_pSkyboxActor = nullptr; // Temp: this should come from a file
		ADirectionalLight* m_pDirectionalLight = nullptr;
		//ASpotLight* m_pSpotLight = nullptr;
		APlayerCharacter* m_pPlayerCharacter = nullptr;
		Vector3 newPos;
		AActor* m_pSelectedActor = nullptr;

		FileSystem m_FileSystem;
		ModelManager m_ModelManager;
		SceneNode* m_pSceneRoot = nullptr;
		std::shared_ptr<RenderingContext> m_Renderer = nullptr;

		std::string m_DisplayName;
	};

}