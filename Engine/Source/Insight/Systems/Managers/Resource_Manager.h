#pragma once

#include <Insight/Core.h>
#include "Insight/Systems/Managers/Model_Manager.h"
#include "Insight/Systems/Managers/Texture_Manager.h"
#include "Insight/Systems/Managers/Mono_Script_Manager.h"

namespace Insight {

	class INSIGHT_API ResourceManager
	{
	public:
		ResourceManager();
		~ResourceManager();

		bool Init();
		virtual bool LoadResourcesFromJson(const rapidjson::Value& jsonResources);

		inline static ResourceManager& Get() { return *s_Instance; }
		void FlushAllResources();

		GeometryManager& GetModelManager() { return *m_pModelManager; }
		TextureManager& GetTextureManager() { return *m_pTextureManager; }
		MonoScriptManager& GetMonoScriptManager() { return *m_pMonoScriptManager; }

	private:
		GeometryManager*	m_pModelManager = nullptr;
		TextureManager* m_pTextureManager = nullptr;
		MonoScriptManager* m_pMonoScriptManager = nullptr;
	private:
		static ResourceManager* s_Instance;
	};

}
