#pragma once

#include <Insight/Core.h>

#include "Scene_Node.h"
#include "Insight/Rendering/Rendering_Context.h"

namespace Insight {


	class INSIGHT_API Scene
	{
	public:
		Scene();
		~Scene();

		bool Init();
		bool PostInit();
		void BeginPlay();
		void OnUpdate(const float& deltaMs);
		void OnPreRender();
		void OnRender();
		void OnPostRender();
		void Destroy();
		void Flush();

	private:
		AActor* m_pTestActor = nullptr;
		

		SceneNode* m_pRoot = nullptr;
		std::shared_ptr<RenderingContext> m_Renderer = nullptr;
	};

}