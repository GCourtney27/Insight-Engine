#include "Renderer.h"
#include <Renderer_pch.h>

#include "Renderer.h"
#include "Insight/Core/Application.h"

#include "Platform/Windows/DirectX_11/Direct3D11_Context.h"
#include "Platform/Windows/DirectX_12/Direct3D12_Context.h"


namespace Insight {


	Renderer* Renderer::s_Instance = nullptr;

	Renderer::Renderer(bool vSyncEabled)
		: m_VSyncEnabled(vSyncEabled)
	{
	}

	void Renderer::HandleEvents()
	{
		while(s_Instance->m_WindowResizeEventQueue.size() != 0)
		{
			s_Instance->OnWindowResize(s_Instance->m_WindowResizeEventQueue.front());
			s_Instance->m_WindowResizeEventQueue.pop();
		}
		while (s_Instance->m_ShaderReloadEventQueue.size() != 0)
		{
			s_Instance->OnShaderReload(s_Instance->m_ShaderReloadEventQueue.front());
			s_Instance->m_ShaderReloadEventQueue.pop();
		}
		while (s_Instance->m_WindowFullScreenEventQueue.size() != 0)
		{
			s_Instance->OnWindowFullScreen(s_Instance->m_WindowFullScreenEventQueue.front());
			s_Instance->m_WindowFullScreenEventQueue.pop();
		}
	}

	Renderer::~Renderer()
	{
	}

	bool Renderer::SetSettingsAndCreateContext(GraphicsSettings GraphicsSettings, Window* pWindow)
	{
		IE_ASSERT(!s_Instance, "Rendering Context already exists! Cannot have more that one context created at a time.");
		IE_ASSERT(pWindow, "Cannot initialize renderer with NULL window context.");

		switch (GraphicsSettings.TargetRenderAPI)
		{
#if defined IE_PLATFORM_WINDOWS
		case eTargetRenderAPI::D3D_11:
		{
			WindowsWindow* Window = (WindowsWindow*)pWindow;
			s_Instance = new Direct3D11Context(Window);
			break;
		}
		case eTargetRenderAPI::D3D_12:
		{
			WindowsWindow* Window = (WindowsWindow*)pWindow;
			s_Instance = new Direct3D12Context(Window);
			break;
		}
#endif // IE_PLATFORM_WINDOWS
		default:
		{
			IE_CORE_ERROR("Failed to create render with given context type: {0}", GraphicsSettings.TargetRenderAPI);
			break;
		}
		}
		s_Instance->m_pWindowRef = pWindow;
		s_Instance->SetGraphicsSettings(GraphicsSettings);

		return s_Instance != nullptr;
	}

	CB_PS_DirectionalLight Renderer::GetDirectionalLightCB() const
	{
		return s_Instance->m_pWorldDirectionalLight->GetConstantBuffer();
	}


	void Renderer::UnRegisterWorldDirectionalLight()
	{
		s_Instance->m_pWorldDirectionalLight = nullptr;
	}

	void Renderer::UnRegisterPointLight(APointLight* pPointLight)
	{
		auto iter = std::find(s_Instance->m_PointLights.begin(), s_Instance->m_PointLights.end(), pPointLight);
		if (iter != s_Instance->m_PointLights.end()) {
			s_Instance->m_PointLights.erase(iter);
		}
	}

	void Renderer::UnRegisterSpotLight(ASpotLight* pSpotLight)
	{
		auto iter = std::find(s_Instance->m_SpotLights.begin(), s_Instance->m_SpotLights.end(), pSpotLight);
		if (iter != s_Instance->m_SpotLights.end()) {
			s_Instance->m_SpotLights.erase(iter);
		}
	}
}