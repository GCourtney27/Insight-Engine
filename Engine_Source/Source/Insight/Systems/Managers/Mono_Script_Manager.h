// Copyright Insight Interactive. All Rights Reserved.
#pragma once
#include <Insight/Core.h>

#if defined (IE_PLATFORM_BUILD_WIN32)

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

namespace Insight {


	namespace Runtime {
		class CSharpScriptComponent;
	}

	class INSIGHT_API MonoScriptManager
	{
	public:
		MonoScriptManager();
		~MonoScriptManager();
		
		bool Init() { return true; }
		bool PostInit() { return true; }
		void OnBeginPlay() {}
		void OnEndPlaySession() {}
		void ReCompile() {}
		void Cleanup() {}

		inline void RegisterScript(Runtime::CSharpScriptComponent* Script) { m_RegisteredScripts.push_back(Script); }
		void UnRegisterScript(Runtime::CSharpScriptComponent* Script) {}

		MonoDomain& GetDomain() { return *m_pDomain; }
		MonoAssembly& GetAssembly() { return *m_pAssembly; }
		MonoImage& GetAssemblyImage() { return *m_pImage; }

		bool CreateClass(MonoClass*& monoClass, MonoObject*& monoObject, const char* className) { return true; }
		bool CreateMethod(MonoClass*& classToInitFrom, MonoMethod*& monoMethod, const char* targetClassName, const char* methodName) { return true; }
		MonoObject* InvokeMethod(MonoMethod*& monoMethod, MonoObject*& monoObject, void* methodArgs[]) { return nullptr; }
		void ImGuiRender() {}

	private:
		MonoDomain* m_pDomain = nullptr;
		MonoAssembly* m_pAssembly = nullptr;
		MonoImage* m_pImage = nullptr;
		
		bool m_ManagerIsInitialized = false;

		const char* m_CSGlobalNamespace = "InsightEngine";
		std::string m_AssemblyDir = "";
		std::vector<Runtime::CSharpScriptComponent*> m_RegisteredScripts;
	};

}
#endif // IE_PLATFORM_BUILD_WIN32
