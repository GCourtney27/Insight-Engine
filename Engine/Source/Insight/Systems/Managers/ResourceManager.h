#pragma once

#include <Insight/Core.h>
#include "Insight/Systems/Managers/GeometryManager.h"
#include "Insight/Systems/Managers/TextureManager.h"
#include "Insight/Systems/Managers/MonoScriptManager.h"

namespace Insight {

	class INSIGHT_API ResourceManager
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
#if defined (IE_PLATFORM_BUILD_WIN32)
		MonoScriptManager& GetMonoScriptManager() { return *m_pMonoScriptManager; }
#endif

	private:
		TextureManager* m_pTextureManager = nullptr;
#if defined (IE_PLATFORM_BUILD_WIN32)
		MonoScriptManager* m_pMonoScriptManager = nullptr;
#endif
	private:
		static ResourceManager* s_Instance;
	};

}
