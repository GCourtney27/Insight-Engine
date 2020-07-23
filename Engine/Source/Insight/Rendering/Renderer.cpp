#include <ie_pch.h>

#include "Renderer.h"
#include "Insight/Core/Application.h"

#include "Platform/Windows/DirectX_11/Direct3D11_Context.h"
#include "Platform/Windows/DirectX_12/Direct3D12_Context.h"


namespace Insight {


	Renderer* Renderer::s_Instance = nullptr;

	Renderer::Renderer(uint32_t windowWidth, uint32_t windowHeight, bool vSyncEabled)
		: m_WindowWidth(windowWidth), m_WindowHeight(windowHeight), m_VSyncEnabled(vSyncEabled)
	{
	}

	Renderer::~Renderer()
	{
	}

	bool Renderer::SetAPIAndCreateContext(eTargetRenderAPI ContextType)
	{
		IE_ASSERT(!s_Instance, "Rendering Context already exists! Cannot have more that one context created at a time.");

		switch (ContextType)
		{
#if defined IE_PLATFORM_WINDOWS
		case eTargetRenderAPI::D3D_11:
		{
			WindowsWindow* Window = (WindowsWindow*)&Application::Get().GetWindow();
			s_Instance = new Direct3D11Context(Window);
			s_Instance->m_CurrentAPI = ContextType;
			break;
		}
		case eTargetRenderAPI::D3D_12:
		{
			WindowsWindow* Window = (WindowsWindow*)&Application::Get().GetWindow();
			s_Instance = new Direct3D12Context(Window);
			s_Instance->m_CurrentAPI = ContextType;
			break;
		}
#endif // IE_PLATFORM_WINDOWS
		default:
		{
			IE_CORE_ERROR("Failed to create render with given context type: {0}", ContextType);
			break;
		}
		}

		return s_Instance != nullptr;
	}


	void Renderer::UnRegisterDirectionalLight(ADirectionalLight* DirectionalLight)
	{
		auto iter = std::find(s_Instance->m_DirectionalLights.begin(), s_Instance->m_DirectionalLights.end(), DirectionalLight);
		if (iter != s_Instance->m_DirectionalLights.end()) {
			s_Instance->m_DirectionalLights.erase(iter);
		}
	}

	void Renderer::UnRegisterPointLight(APointLight* PointLight)
	{
		auto iter = std::find(s_Instance->m_PointLights.begin(), s_Instance->m_PointLights.end(), PointLight);
		if (iter != s_Instance->m_PointLights.end()) {
			s_Instance->m_PointLights.erase(iter);
		}
	}

	void Renderer::UnRegisterSpotLight(ASpotLight* SpotLight)
	{
		auto iter = std::find(s_Instance->m_SpotLights.begin(), s_Instance->m_SpotLights.end(), SpotLight);
		if (iter != s_Instance->m_SpotLights.end()) {
			s_Instance->m_SpotLights.erase(iter);
		}
	}
}