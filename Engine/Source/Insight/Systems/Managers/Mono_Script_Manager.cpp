#include <ie_pch.h>

#include "Mono_Script_Manager.h"

#include "Insight/Systems/File_System.h"
#include "Insight/Systems/Managers/Resource_Manager.h"
#include "Insight/Input/Windows_Input.h"

#include "Insight/Runtime/Components/CSharp_Scirpt_Component.h"

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
		monoClass = mono_class_from_name(m_pImage, m_CSGlobalNamespace, className);
		monoObject = mono_object_new(m_pDomain, monoClass);
		mono_runtime_object_init(monoObject);

		return (monoClass && monoObject);
	}

	bool MonoScriptManager::CreateMethod(MonoClass*& classToInitFrom, MonoMethod*& monoMethod, const char* targetClassName, const char* methodName)
	{
		MonoMethodDesc* methodDesc;
		std::string methodSignature;
		methodSignature = m_CSGlobalNamespace;
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
		const char* BuildConfig = MACRO_TO_STRING(IE_BUILD_CONFIG);
		m_AssemblyDir = FileSystem::ProjectDirectory;
		m_AssemblyDir += "/Bin/";
		m_AssemblyDir += BuildConfig;
		m_AssemblyDir += "/Assembly-CSharp/Assembly-CSharp.dll";

		mono_set_dirs("C:/Program Files/Mono/lib", "C:/Program Files/Mono/etc");

		m_pDomain = mono_jit_init("IE-Mono-Script-Engine");
		if (!m_pDomain) {
			IE_CORE_ERROR("Failed to initialize mono domain.");
			return false;
		}
		
		
		//m_pAssembly = mono_domain_assembly_open(m_pDomain, m_AssemblyDir.c_str());
		m_pAssembly = mono_domain_assembly_open(m_pDomain, m_AssemblyDir.c_str());
		if (!m_pAssembly) {
			IE_CORE_ERROR("Failed to open mono assembly with path: {0}", m_AssemblyDir);
			return false;
		}

		/*m_AssemblyDir = FileSystem::ProjectDirectory;
		m_AssemblyDir += "/Bin/";
		m_AssemblyDir += BuildConfig;
		m_AssemblyDir += "/Assembly-CSharp/";
		MonoImageOpenStatus* Status = nullptr;
		m_pImage = mono_image_open_from_data_with_name(reinterpret_cast<char*>(m_pImage), sizeof(m_pImage), true, Status, true, m_AssemblyDir.c_str());*/


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

	bool MonoScriptManager::PostInit()
	{
		
		return true;
	}

	void MonoScriptManager::ReCompile()
	{
		for (CSharpScriptComponent*& Script : m_RegisteredScripts) {

			Script->Cleanup();
		}

		mono_images_cleanup();
		//mono_image_close(m_pImage);
		mono_assemblies_cleanup();
		//mono_assembly_close(m_pAssembly);

		/*if (!AssemblyClosed) {
			AssemblyClosed = true;
			mono_assembly_close(m_pAssembly);
			mono_assemblies_cleanup();
			return;
		}

		if (AssemblyClosed) {
			AssemblyClosed = false;
		}*/
		//Cleanup();
		
		//mono_image_open_from_data()
		//mono_image_open_from_data_full()
		//mono_image_open_from_data_with_name()

		m_pAssembly = mono_domain_assembly_open(m_pDomain, m_AssemblyDir.c_str());
		if (!m_pAssembly) {
			IE_CORE_ERROR("Failed to open mono assembly with path: \"{0}\" during recompile", m_AssemblyDir);
		}

		m_pImage = mono_assembly_get_image(m_pAssembly);
		if (!m_pImage) {
			IE_CORE_ERROR("Failed to get image from mono assembly during recompile.");
		}

		// Register C# -> C++ calls
		// Input
		mono_add_internal_call("Internal.Input::IsKeyPressed", reinterpret_cast<const void*>(Interop_IsKeyPressed));
		mono_add_internal_call("Internal.Input::IsMouseButtonPressed", reinterpret_cast<const void*>(Interop_IsMouseButtonPressed));
		mono_add_internal_call("Internal.Input::GetMouseX", reinterpret_cast<const void*>(Interop_GetMouseX));
		mono_add_internal_call("Internal.Input::GetMouseY", reinterpret_cast<const void*>(Interop_GetMouseY));
		for (CSharpScriptComponent*& Script : m_RegisteredScripts) {

			Script->RegisterScript();
		}
	}

	void MonoScriptManager::Cleanup()
	{
		mono_image_close(m_pImage);
		mono_assemblies_cleanup();
		mono_jit_cleanup(m_pDomain);
		mono_images_cleanup();
		mono_assembly_close(m_pAssembly);
		m_pDomain = nullptr;
		m_pAssembly = nullptr;
		m_pImage = nullptr;
	}

	void MonoScriptManager::UnRegisterScript(CSharpScriptComponent* Script)
	{
		auto iter = std::find(m_RegisteredScripts.begin(), m_RegisteredScripts.end(), Script);

		if (iter != m_RegisteredScripts.end()) {
			m_RegisteredScripts.erase(iter);
		}
	}

}