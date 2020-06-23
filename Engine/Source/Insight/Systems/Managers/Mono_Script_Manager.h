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
		void ReCompile();
		void Cleanup();


		MonoDomain& GetDomain() { return *m_pDomain; }
		MonoAssembly& GetAssembly() { return *m_pAssembly; }
		MonoImage& GetAssemblyImage() { return *m_pImage; }

		bool CreateClass(MonoClass*& monoClass, MonoObject*& monoObject, const char* className);
		bool CreateMethod(MonoClass*& classToInitFrom, MonoMethod*& monoMethod, const char* targetClassName, const char* methodName);
		void InvokeMethod(MonoMethod*& monoMethod, MonoObject*& monoObject, void* methodArgs[]);
		void ImGuiRender();

	private:
		MonoDomain* m_pDomain = nullptr;
		MonoAssembly* m_pAssembly = nullptr;
		MonoImage* m_pImage = nullptr;
		
		const char* m_CSCGlobalNamespace = "InsightEngine";

		// TEMP
		MonoClass* monoClass;
		MonoObject* monoObject;
		MonoMethod* method;
	};

}

