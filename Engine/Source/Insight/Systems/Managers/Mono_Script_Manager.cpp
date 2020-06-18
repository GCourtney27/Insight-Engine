#include <ie_pch.h>

#include "Mono_Script_Manager.h"
#include "Insight/Systems/Managers/Resource_Manager.h"

namespace Insight {

	MonoScriptManager::MonoScriptManager()
	{

	}

	MonoScriptManager::~MonoScriptManager()
	{
		Cleanup();
	}

	MonoString* Interop_PrintCPPMsg()
	{
		return mono_string_new(mono_domain_get(), "String From C++");
	}

	bool MonoScriptManager::Init()
	{
		const char* libDir = "Vendor/Mono/lib";
		const char* etcDir = "Vendor/Mono/etc";

		mono_set_dirs(libDir, etcDir);

		m_pDomain = mono_jit_init("IE-Mono-Script-Engine");
		if (!m_pDomain) {
			IE_CORE_ERROR("Failed to initialize mono domain.");
			return false;
		}

		// TODO fix assemblyPath not changing in release and debug configurations
		std::string assemblyDir;
#if defined IE_DEBUG
		char* relativeDir = MACRO_TO_STRING(IE_BUILD_DIR);
		assemblyDir = relativeDir;
		assemblyDir += "Assembly-CSharp.dll";
#elif defined IE_RELEASE
		assemblyPath = "Assembly-CSharp.dll";
#endif
		m_pAssembly = mono_domain_assembly_open(m_pDomain, assemblyDir.c_str());
		if (!m_pAssembly) {
			IE_CORE_ERROR("Failed to open mono assembly with path: {0}", assemblyDir);
			return false;
		}

		MonoImage* image = mono_assembly_get_image(m_pAssembly);
		if (!image) {
			IE_CORE_ERROR("Failed to get image from mono assembly.");
			return false;
		}

		mono_add_internal_call("Source.Test::PrintCPPMsg", reinterpret_cast<const void*>(Interop_PrintCPPMsg));

		// DEMO
		{
			MonoClass* monoClass = mono_class_from_name(image, "Source", "Test");
			MonoObject* monoObject = mono_object_new(m_pDomain, monoClass);
			mono_runtime_object_init(monoObject);
			
			MonoMethodDesc* TypeMethodDesc;
			const char* TypeMethodDescStr = "Source.Test::Method()";
			TypeMethodDesc = mono_method_desc_new(TypeMethodDescStr, false);
			if (!TypeMethodDesc) {
				IE_CORE_ERROR("Failed to find method signature in mono image.");
				return false;
			}

			MonoMethod* method = mono_method_desc_search_in_class(TypeMethodDesc, monoClass);

			mono_runtime_invoke(method, monoObject, nullptr, nullptr);
		}
		

		return true;
	}


	void MonoScriptManager::Cleanup()
	{
		mono_jit_cleanup(m_pDomain);
	}

}