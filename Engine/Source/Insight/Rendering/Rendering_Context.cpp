#include <ie_pch.h>

#include "Rendering_Context.h"
#include "Insight/Core/Application.h"

#include "Platform/Windows/DirectX_11/Direct3D11_Context.h"
#include "Platform/Windows/DirectX_12/Direct3D12_Context.h"

namespace Insight {


	RenderingContext* RenderingContext::s_Instance = nullptr;

	RenderingContext::RenderingContext(UINT windowWidth, UINT windowHeight, bool vSyncEabled)
		: m_WindowWidth(windowWidth), m_WindowHeight(windowHeight), m_VSyncEnabled(vSyncEabled)
	{
	}

	RenderingContext::~RenderingContext()
	{
	}

	bool RenderingContext::SetContext(RenderingAPI ContextType)
	{
		IE_ASSERT(!s_Instance, "Rendering Context already exists! Cannot have more that one context created at a time.");

		switch (ContextType)
		{
		case RenderingAPI::D3D_11:
		{
			WindowsWindow* Window = (WindowsWindow*)&Application::Get().GetWindow();
			s_Instance = new Direct3D11Context(Window);
			break;
		}
		case RenderingAPI::D3D_12:
		{
			WindowsWindow* Window = (WindowsWindow*)&Application::Get().GetWindow();
			s_Instance = new Direct3D12Context(Window);
			break;
		}
		default:
		{
			IE_CORE_ERROR("Failed to create render with given context type: {0}", ContextType);
			break;
		}
		}
		s_Instance->m_CurrentAPI = ContextType;

		return s_Instance != nullptr;
	}

}