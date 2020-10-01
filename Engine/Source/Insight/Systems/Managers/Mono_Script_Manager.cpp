#include <Engine_pch.h>

#include "Mono_Script_Manager.h"

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

	bool MonoScriptManager::CreateMethod(MonoClass*& ClassToInitFrom, MonoMethod*& MonoMethod, const char* TargetClassName, const char* MethodName)
	{
		MonoMethodDesc* MethodDesc;
		std::string MethodSignature;
		MethodSignature = m_CSGlobalNamespace;
		MethodSignature += ".";
		MethodSignature += TargetClassName;
		MethodSignature += ":";
		MethodSignature += MethodName;
		MethodDesc = mono_method_desc_new(MethodSignature.c_str(), true);
		MonoMethod = mono_method_desc_search_in_class(MethodDesc, ClassToInitFrom);
		mono_method_desc_free(MethodDesc);

		return (MonoMethod);
	}

	MonoObject* MonoScriptManager::InvokeMethod(MonoMethod*& MethodToCall, MonoObject*& BelongingObject, void* MethodArgs[])
	{
		return mono_runtime_invoke(MethodToCall, BelongingObject, MethodArgs, nullptr);
	}

	void MonoScriptManager::ImGuiRender()
	{
		ImGui::Begin("DEBUG: Mono Script Manager");
		{
		}
		ImGui::End();
	}

	mono_bool Interop_IsKeyPressed(char KeyCode)
	{
		mono_bool pressed = Input::IsKeyPressed(KeyCode);
		return pressed;
	}

	mono_bool Interop_IsMouseButtonPressed(int MouseButton)
	{
		mono_bool pressed = Input::IsMouseButtonPressed(MouseButton);
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
		if (!m_ManagerIsInitialized) {

			const char* BuildConfig = MACRO_TO_STRING(RN_BUILD_CONFIG);
			m_AssemblyDir = FileSystem::GetProjectDirectory();
			m_AssemblyDir += "/Bin/";
			m_AssemblyDir += BuildConfig;
			m_AssemblyDir += "/Assembly-CSharp/Assembly-CSharp.dll";

			mono_set_dirs("C:/Program Files/Mono/lib", "C:/Program Files/Mono/etc");

			m_pDomain = mono_jit_init_version("IE-Mono-Script-Engine", "v4.0.30319");
			if (!m_pDomain) {
				IE_CORE_ERROR("Failed to initialize mono domain.");
				return false;
			}
			m_ManagerIsInitialized = true;

		}
		m_pDomain = mono_domain_create_appdomain("IE-Mono-Script-Engine", NULL);
		mono_domain_set(m_pDomain, false);

		m_pAssembly = mono_domain_assembly_open(m_pDomain, m_AssemblyDir.c_str());
		if (!m_pAssembly) {
			IE_CORE_ERROR("Failed to open mono assembly with path: {0}", m_AssemblyDir);
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

	bool MonoScriptManager::PostInit()
	{
		/*MonoDomain* domainToUnload = mono_domain_get();
		if (domainToUnload && domainToUnload != mono_get_root_domain())
		{
			mono_domain_set(mono_get_root_domain(), false);
			mono_domain_unload(domainToUnload);
		}*/

		mono_assembly_close(m_pAssembly);

		return true;
	}

	void MonoScriptManager::OnBeginPlay()
	{
		Init();

	}

	void MonoScriptManager::OnEndPlaySession()
	{
		mono_domain_unload(m_pDomain);
		mono_assemblies_cleanup();

	}

	void MonoScriptManager::ReCompile()
	{
		// TODO Unload while in editor and reload when playing
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

	void MonoScriptManager::UnRegisterScript(Runtime::CSharpScriptComponent* Script)
	{
		auto iter = std::find(m_RegisteredScripts.begin(), m_RegisteredScripts.end(), Script);

		if (iter != m_RegisteredScripts.end()) {
			m_RegisteredScripts.erase(iter);
		}
	}

}