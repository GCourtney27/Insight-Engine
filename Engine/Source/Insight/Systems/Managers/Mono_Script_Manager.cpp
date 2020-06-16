#include <ie_pch.h>

#include "Mono_Script_Manager.h"

namespace Insight {

	MonoScriptManager::MonoScriptManager()
	{

	}

	MonoScriptManager::~MonoScriptManager()
	{
		Destroy();
	}

	bool MonoScriptManager::Init()
	{
//#if defined IE_DEBUG
//		LPCWSTR vertexShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/Geometry_Pass.vertex.cso";
//		LPCWSTR pixelShaderFolder = L"../Bin/Debug-windows-x86_64/Engine/Geometry_Pass.pixel.cso";
//#elif defined IE_RELEASE
//		LPCWSTR vertexShaderFolder = L"Geometry_Pass.vertex.cso";
//		LPCWSTR pixelShaderFolder = L"Geometry_Pass.pixel.cso";
//#endif 
		const char* libDir = "Vendor/Mono/lib";
		const char* etcDir = "Vendor/Mono/etc";

		mono_set_dirs(libDir, etcDir);

		m_pDomain = mono_jit_init("Insight-Mono-Script-Engine");
		if (!m_pDomain) {
			IE_CORE_ERROR("Failed to initialize mono domain.");
			return false;
		}
#if defined IE_DEBUG
		const char* assemblyPath = MACRO_TO_STRING(IE_BUILD_DIR) "Assembly-CSharp.dll";
#elif defined IE_RELEASE
		char* assemblyPath = "Assembly-CSharp.dll";
#endif
		m_pAssembly = mono_domain_assembly_open(m_pDomain, "../Bin/Debug-windows-x86_64/Engine/Assembly-CSharp.dll");
		if (!m_pAssembly) {
			IE_CORE_ERROR("Failed to initialize mono assembly.");
			Destroy();
			return false;
		}

		MonoImage* image;
		image = mono_assembly_get_image(m_pAssembly);
		if (!image) {
			IE_CORE_ERROR("Failed to get image from mono assembly.");
			return false;
		}

		// DEMO
		{
			MonoMethodDesc* TypeMethodDesc;
			const char* TypeMethodDescStr = "Test::GetNumber()";
			TypeMethodDesc = mono_method_desc_new(TypeMethodDescStr, false);
			if (!TypeMethodDesc) {
				IE_CORE_ERROR("Failed to find method signature in mono image.");
				return false;
			}

			//Search the method in the image
			MonoMethod* method;
			method = mono_method_desc_search_in_image(TypeMethodDesc, image);
			if (!method) {
				IE_CORE_ERROR("Failed to get method from mono image.");
				return false;
			}

			// run the method
			MonoObject* result = mono_runtime_invoke(method, nullptr, nullptr, nullptr);
			int num = *(int*)mono_object_unbox(result);
			IE_CORE_INFO("Mono result: {0}", num);
		}

		return true;
	}

	void MonoScriptManager::Destroy()
	{
		mono_jit_cleanup(m_pDomain);
	}

}