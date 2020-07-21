#include <ie_pch.h>

#include "Direct3D11_Context.h"

#include "Insight/Core/Application.h"
#include "Platform/Windows/Windows_Window.h"
#include "Insight/Runtime/APlayer_Character.h"
#include "Insight/Systems/Managers/Geometry_Manager.h"

#include "Insight/Rendering/APost_Fx.h"
#include "Insight/Rendering/ASky_Light.h"
#include "Insight/Rendering/ASky_Sphere.h"
#include "Insight/Rendering/Lighting/ASpot_Light.h"
#include "Insight/Rendering/Lighting/APoint_Light.h"
#include "Insight/Rendering/Lighting/ADirectional_Light.h"

namespace Insight {



	bool Direct3D11Context::SetVertexBuffers(uint32_t StartSlot, uint32_t NumBuffers, VertexBuffer Buffer)
	{
		return false;
	}

	bool Direct3D11Context::SetIndexBuffers(IndexBuffer Buffer)
	{
		return false;
	}

	bool Direct3D11Context::DrawIndexedInstanced(uint32_t IndexCountPerInstance, uint32_t NumInstances, uint32_t StartIndexLocation, uint32_t BaseVertexLoaction, uint32_t StartInstanceLocation)
	{
		return false;
	}

	Direct3D11Context::Direct3D11Context(WindowsWindow* WindowHandle) 
		: m_pWindowHandle(&WindowHandle->GetWindowHandleReference()),
		m_pWindow(WindowHandle),
		RenderingContext(WindowHandle->GetWidth(), WindowHandle->GetHeight(), false)

	{
	}

	Direct3D11Context::~Direct3D11Context()
	{
	}

	bool Direct3D11Context::Init()
	{
		IE_CORE_INFO("Renderer: D3D 11");

		return false;
	}

	bool Direct3D11Context::PostInit()
	{
		return false;
	}

	void Direct3D11Context::OnUpdate(const float& deltaTime)
	{
	}

	void Direct3D11Context::OnPreFrameRender()
	{
	}

	void Direct3D11Context::OnRender()
	{
	}

	void Direct3D11Context::OnMidFrameRender()
	{
	}

	void Direct3D11Context::ExecuteDraw()
	{
	}

	void Direct3D11Context::SwapBuffers()
	{
	}

	void Direct3D11Context::OnWindowResize()
	{
	}

	void Direct3D11Context::OnWindowFullScreen()
	{
	}

}