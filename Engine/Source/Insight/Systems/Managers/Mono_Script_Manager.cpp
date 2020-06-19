#include <ie_pch.h>

#include "Mono_Script_Manager.h"
#include "Insight/Systems/Managers/Resource_Manager.h"
#include "imgui.h"

namespace Insight {

	MonoScriptManager::MonoScriptManager()
	{
	}

	MonoScriptManager::~MonoScriptManager()
	{
		Cleanup();
	}

	bool MonoScriptManager::CreateClass(MonoClass* monoClass, MonoObject* monoObject, const char* className)
	{
		monoClass = mono_class_from_name(m_pImage, "", className);
		monoObject = mono_object_new(m_pDomain, monoClass);
		mono_runtime_object_init(monoObject);

		return (monoClass && monoObject);
	}

	bool MonoScriptManager::CreateMethod(MonoClass* classToInitFrom, MonoMethod* monoMethod, const char* targetClassName, const char* methodName)
	{
		MonoMethodDesc* methodDesc;
		std::string methodSignature;
		size_t strSize = ((sizeof(*methodName) + sizeof(*targetClassName) + (2 * sizeof(char))) / sizeof(char));
		methodSignature.reserve(strSize);
		methodSignature = targetClassName;
		methodSignature += "::";
		methodSignature += methodName;
		methodDesc = mono_method_desc_new(methodSignature.c_str(), false);
		monoMethod = mono_method_desc_search_in_class(methodDesc, classToInitFrom);

		return (monoMethod);
	}

	void MonoScriptManager::InvokeMethod(MonoMethod* monoMethod, MonoObject* monoObject, void* methodArgs[])
	{
		mono_runtime_invoke(monoMethod, monoObject, methodArgs, nullptr);
	}

	void MonoScriptManager::ImGuiRender()
	{
		ImGui::Begin("DEBUG: Mono Script Manager");
		{
			if (ImGui::Button("Re-Compile", ImVec2{ 100.0f, 50.0f }))
			{
				ReCompile();
			}
		}
		ImGui::End();
	}

	void Interop_SetPosition(float monoX, float monoY, float monoZ)
	{
		IE_CORE_INFO("x: {0} - y: {1} - z: {2}", monoX, monoY, monoZ);
	}

	void Interop_PrintCPPMsg(MonoString* msg)
	{
		char* msgctrs = mono_string_to_utf8(msg);
		IE_CORE_INFO("C++ Called from C#: {0}", msgctrs);
		//return mono_string_new(mono_domain_get(), "String From C++");
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

		m_pImage = mono_assembly_get_image(m_pAssembly);
		if (!m_pImage) {
			IE_CORE_ERROR("Failed to get image from mono assembly.");
			return false;
		}

		mono_add_internal_call("Source.Interop::PrintCPPMsg", reinterpret_cast<const void*>(Interop_PrintCPPMsg));
		mono_add_internal_call("Source.Interop::SetPosition", reinterpret_cast<const void*>(Interop_SetPosition));

		// DEMO
		{
			MonoClass* monoClass = mono_class_from_name(m_pImage, "Source", "Test");
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

	void MonoScriptManager::ReCompile()
	{
		Cleanup();
		Init();
	}

	void MonoScriptManager::Cleanup()
	{
		mono_jit_cleanup(m_pDomain);
		/*mono_assemblies_cleanup();
		mono_images_cleanup();
		mono_assembly_close(m_pAssembly);
		mono_image_close(m_pImage);*/
		m_pDomain = nullptr;
		m_pAssembly = nullptr;
		m_pImage = nullptr;
	}

}