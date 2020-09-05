#include <ie_pch.h>

#include "Geometry_Manager.h"

#include "Insight/Rendering/Renderer.h"
#include "Insight/Runtime/APlayer_Character.h"

#include "Platform/Windows/DirectX_12/Direct3D12_Context.h"
#include "Platform/Windows/DirectX_11/Geometry/D3D11_Geometry_Manager.h"
#include "Platform/Windows/DirectX_12/Geometry/D3D12_Geometry_Manager.h"

#include <fstream>

namespace Insight {

	GeometryManager* GeometryManager::s_Instance = nullptr;

	GeometryManager::GeometryManager()
	{
	}

	GeometryManager::~GeometryManager()
	{
		Shutdown();
	}

	bool GeometryManager::InitGlobalInstance()
	{
		IE_ASSERT(!s_Instance, "An instance of Geometry Manager exists!");

		switch (Renderer::GetAPI())
		{
		case Renderer::eTargetRenderAPI::D3D_11:
		{
			s_Instance = new D3D11GeometryManager();
			break;
		}
		case Renderer::eTargetRenderAPI::D3D_12:
		{
			s_Instance = new D3D12GeometryManager();
			break;
		}
		default:
		{
			IE_CORE_FATAL(L"Failed to determine graphics api to initialize geometry manager. The render may have not been initialized properly or may not have been initialized at all.");
			break;
		}
		}

		return (s_Instance != nullptr);
	}

	void GeometryManager::Shutdown()
	{
		FlushModelCache();
	}

	void GeometryManager::FlushModelCache()
	{
		s_Instance->m_OpaqueModels.clear();
	}

	void GeometryManager::UnRegisterOpaqueModel(StrongModelPtr Model)
	{
		auto iter = std::find(s_Instance->m_OpaqueModels.begin(), s_Instance->m_OpaqueModels.end(), Model);

		if (iter != s_Instance->m_OpaqueModels.end()) {
			s_Instance->m_OpaqueModels.erase(iter);
		}
	}

	void GeometryManager::UnRegisterTranslucentModel(StrongModelPtr Model)
	{
		auto iter = std::find(s_Instance->m_TranslucentModels.begin(), s_Instance->m_TranslucentModels.end(), Model);

		if (iter != s_Instance->m_TranslucentModels.end()) {
			s_Instance->m_TranslucentModels.erase(iter);
		}
	}

}

