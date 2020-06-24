#include <ie_pch.h>

#include "Mono_Script_Manager.h"
#include "Insight/Systems/Managers/Resource_Manager.h"
#include "Insight/Input/Windows_Input.h"
#include "imgui.h"

namespace Insight {

	MonoScriptManager::MonoScriptManager()
	{
	}

	MonoScriptManager::~MonoScriptManager()
	{
		Cleanup();
	}

	bool MonoScriptManager::CreateClass(MonoClass*& monoClass, MonoObject*& monoObject, const char* className)
	{
		monoClass = mono_class_from_name(m_pImage, m_CSCGlobalNamespace, className);
		monoObject = mono_object_new(m_pDomain, monoClass);
		mono_runtime_object_init(monoObject);

		return (monoClass && monoObject);
	}

	bool MonoScriptManager::CreateMethod(MonoClass*& classToInitFrom, MonoMethod*& monoMethod, const char* targetClassName, const char* methodName)
	{
		MonoMethodDesc* methodDesc;
		std::string methodSignature;
		methodSignature = m_CSCGlobalNamespace;
		methodSignature += ".";
		methodSignature += targetClassName;
		methodSignature += ":";
		methodSignature += methodName;
		methodDesc = mono_method_desc_new(methodSignature.c_str(), true);
		monoMethod = mono_method_desc_search_in_class(methodDesc, classToInitFrom);
		mono_method_desc_free(methodDesc);

		return (monoMethod);
	}

	MonoObject* MonoScriptManager::InvokeMethod(MonoMethod*& methodToCall, MonoObject*& belongingObject, void* methodArgs[])
	{
		return mono_runtime_invoke(methodToCall, belongingObject, methodArgs, nullptr);
	}

	void MonoScriptManager::ImGuiRender()
	{
		ImGui::Begin("DEBUG: Mono Script Manager");
		{
			/*if (ImGui::Button("Close Assemblies", ImVec2{ 100.0f, 50.0f }))
			{
				mono_images_cleanup();
				mono_image_close(m_pImage);
				mono_assemblies_cleanup();
				mono_assembly_close(m_pAssembly);

			}
			if (ImGui::Button("Re-Compile", ImVec2{ 100.0f, 50.0f }))
			{
				ReCompile();
			}*/
		}
		ImGui::End();
	}

	mono_bool Interop_IsKeyPressed(char keyCode)
	{
		mono_bool pressed = Input::IsKeyPressed(keyCode);
		return pressed;
	}

	mono_bool Interop_IsMouseButtonPressed(int mouseButton)
	{
		mono_bool pressed = Input::IsMouseButtonPressed(mouseButton);
		return pressed;
	}

	int Interop_GetMouseX()
	{
		return (int)Input::GetMouseX();
	}

	int Interop_GetMouseY()
	{
		return (int)Input::GetMouseY();
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

		// Register C# -> C++ calls
		// Input
		mono_add_internal_call("Internal.Input::IsKeyPressed", reinterpret_cast<const void*>(Interop_IsKeyPressed));
		mono_add_internal_call("Internal.Input::IsMouseButtonPressed", reinterpret_cast<const void*>(Interop_IsMouseButtonPressed));
		mono_add_internal_call("Internal.Input::GetMouseX", reinterpret_cast<const void*>(Interop_GetMouseX));
		mono_add_internal_call("Internal.Input::GetMouseY", reinterpret_cast<const void*>(Interop_GetMouseY));

		return true;
	}

	void MonoScriptManager::ReCompile()
	{
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
			IE_CORE_ERROR("Failed to open mono assembly with path: \"{0}\" durint recompile", assemblyDir);
		}

		m_pImage = mono_assembly_get_image(m_pAssembly);
		if (!m_pImage) {
			IE_CORE_ERROR("Failed to get image from mono assembly during recompile.");
		}

	}

	void MonoScriptManager::Cleanup()
	{
		mono_jit_cleanup(m_pDomain);
		/*mono_assemblies_cleanup();
		mono_images_cleanup();
		mono_assembly_close(m_pAssembly);
		mono_image_close(m_pImage);
		m_pDomain = nullptr;
		m_pAssembly = nullptr;
		m_pImage = nullptr;*/
	}

}