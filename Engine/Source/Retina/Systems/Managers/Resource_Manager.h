#pragma once

#include <Retina/Core.h>
#include "Retina/Systems/Managers/Geometry_Manager.h"
#include "Retina/Systems/Managers/Texture_Manager.h"
#include "Retina/Systems/Managers/Mono_Script_Manager.h"

namespace Retina {

	class RETINA_API ResourceManager
	{
	public:
		ResourceManager();
		~ResourceManager();

		bool Init();
		bool PostAppInit();
		virtual bool LoadResourcesFromJson(const rapidjson::Value& jsonResources);

		inline static ResourceManager& Get() { return *s_Instance; }
		void FlushAllResources();

		TextureManager& GetTextureManager() { return *m_pTextureManager; }
		MonoScriptManager& GetMonoScriptManager() { return *m_pMonoScriptManager; }

	private:
		TextureManager* m_pTextureManager = nullptr;
		MonoScriptManager* m_pMonoScriptManager = nullptr;
	private:
		static ResourceManager* s_Instance;
	};

}
