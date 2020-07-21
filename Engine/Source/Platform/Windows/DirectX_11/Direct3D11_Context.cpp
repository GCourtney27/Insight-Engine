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




	Direct3D11Context::Direct3D11Context(WindowsWindow* WindowHandle) 
		: m_pWindowHandle(&WindowHandle->GetWindowHandleReference()),
		m_pWindow(WindowHandle),
		RenderingContext(WindowHandle->GetWidth(), WindowHandle->GetHeight(), false)

	{
	}

	Direct3D11Context::~Direct3D11Context()
	{
	}

	bool Direct3D11Context::InitImpl()
	{
		IE_CORE_INFO("Renderer: D3D 11");

		return false;
	}

	void Direct3D11Context::SetVertexBuffersImpl(uint32_t StartSlot, uint32_t NumBuffers, VertexBuffer Buffer)
	{
	}

	void Direct3D11Context::SetIndexBuffersImpl(IndexBuffer Buffer)
	{
	}

	void Direct3D11Context::DrawIndexedInstancedImpl(uint32_t IndexCountPerInstance, uint32_t NumInstances, uint32_t StartIndexLocation, uint32_t BaseVertexLoaction, uint32_t StartInstanceLocation)
	{
	}

	void Direct3D11Context::DestroyImpl()
	{
	}

	bool Direct3D11Context::PostInitImpl()
	{
		return false;
	}

	void Direct3D11Context::OnUpdateImpl(const float DeltaMs)
	{
	}

	void Direct3D11Context::OnPreFrameRenderImpl()
	{
	}

	void Direct3D11Context::OnRenderImpl()
	{
	}

	void Direct3D11Context::OnMidFrameRenderImpl()
	{
	}

	void Direct3D11Context::ExecuteDrawImpl()
	{
	}

	void Direct3D11Context::SwapBuffersImpl()
	{
	}

	void Direct3D11Context::OnWindowResizeImpl()
	{
	}

	void Direct3D11Context::OnWindowFullScreenImpl()
	{
	}

}