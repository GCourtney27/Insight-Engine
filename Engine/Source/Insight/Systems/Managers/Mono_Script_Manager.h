#pragma once
#include <Insight/Core.h>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

namespace Insight {

	class INSIGHT_API MonoScriptManager
	{
	public:
		MonoScriptManager();
		~MonoScriptManager();
		
		bool Init();
		void Destroy();

	private:
		MonoDomain* m_pDomain = nullptr;
		MonoAssembly* m_pAssembly = nullptr;
	};

}

