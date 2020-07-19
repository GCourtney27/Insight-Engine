#pragma once
#include <Insight/Core.h>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

namespace Insight {

	class CSharpScriptComponent;

	class INSIGHT_API MonoScriptManager
	{
	public:
		MonoScriptManager();
		~MonoScriptManager();
		
		bool Init();
		bool PostInit();
		void ReCompile();
		void Cleanup();

		inline void RegisterScript(CSharpScriptComponent* Script) { m_RegisteredScripts.push_back(Script); }
		void UnRegisterScript(CSharpScriptComponent* Script);

		MonoDomain& GetDomain() { return *m_pDomain; }
		MonoAssembly& GetAssembly() { return *m_pAssembly; }
		MonoImage& GetAssemblyImage() { return *m_pImage; }

		bool CreateClass(MonoClass*& monoClass, MonoObject*& monoObject, const char* className);
		bool CreateMethod(MonoClass*& classToInitFrom, MonoMethod*& monoMethod, const char* targetClassName, const char* methodName);
		MonoObject* InvokeMethod(MonoMethod*& monoMethod, MonoObject*& monoObject, void* methodArgs[]);
		void ImGuiRender();

	private:
		MonoDomain* m_pDomain = nullptr;
		MonoAssembly* m_pAssembly = nullptr;
		MonoImage* m_pImage = nullptr;
		
		bool AssemblyClosed = false;

		const char* m_CSGlobalNamespace = "InsightEngine";
		std::string m_AssemblyDir = "";
		std::vector<CSharpScriptComponent*> m_RegisteredScripts;
	};

}

