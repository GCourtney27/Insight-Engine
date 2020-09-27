#include <Renderer_pch.h>

#include "Renderer.h"
#include "Retina/Core/Application.h"

#include "Platform/Windows/DirectX_11/Direct3D11_Context.h"
#include "Platform/Windows/DirectX_12/Direct3D12_Context.h"


namespace Retina {


	Renderer* Renderer::s_Instance = nullptr;

	Renderer::Renderer(uint32_t windowWidth, uint32_t windowHeight, bool vSyncEabled)
		: m_WindowWidth(windowWidth), m_WindowHeight(windowHeight), m_VSyncEnabled(vSyncEabled)
	{
	}

	Renderer::~Renderer()
	{
	}

	bool Renderer::SetSettingsAndCreateContext(GraphicsSettings GraphicsSettings)
	{
		RN_ASSERT(!s_Instance, "Rendering Context already exists! Cannot have more that one context created at a time.");


		switch (GraphicsSettings.TargetRenderAPI)
		{
#if defined RN_PLATFORM_WINDOWS
		case eTargetRenderAPI::D3D_11:
		{
			WindowsWindow* Window = (WindowsWindow*)&Application::Get().GetWindow();
			s_Instance = new Direct3D11Context(Window);
			break;
		}
		case eTargetRenderAPI::D3D_12:
		{
			WindowsWindow* Window = (WindowsWindow*)&Application::Get().GetWindow();
			s_Instance = new Direct3D12Context(Window);
			break;
		}
#endif // RN_PLATFORM_WINDOWS
		default:
		{
			RN_CORE_ERROR("Failed to create render with given context type: {0}", GraphicsSettings.TargetRenderAPI);
			break;
		}
		}
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