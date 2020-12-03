#include <Engine_pch.h>

#include "D3D12_ImGui_Layer.h"

#include "Insight/Core/Application.h"

#include "Platform/DirectX_12/Direct3D12_Context.h"
#include "Platform/Win32/Win32_Window.h"

#if defined (IE_PLATFORM_BUILD_WIN32)
#include "imgui.h"
#include <examples/imgui_impl_dx12.cpp>
#include "examples/imgui_impl_dx12.h"
#include "examples/imgui_impl_win32.h"
#endif
namespace Insight {


	void D3D12ImGuiLayer::OnAttach()
	{
		Super::OnAttach();

#if defined (IE_PLATFORM_BUILD_WIN32)
		// Set ImGui Key Bindings
		{
			m_pIO->KeyMap[ImGuiKey_Tab] = VK_TAB;
			m_pIO->KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
			m_pIO->KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
			m_pIO->KeyMap[ImGuiKey_UpArrow] = VK_UP;
			m_pIO->KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
			m_pIO->KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
			m_pIO->KeyMap[ImGuiKey_PageDown] = VK_NEXT;
			m_pIO->KeyMap[ImGuiKey_Home] = VK_HOME;
			m_pIO->KeyMap[ImGuiKey_End] = VK_END;
			m_pIO->KeyMap[ImGuiKey_Insert] = VK_INSERT;
			m_pIO->KeyMap[ImGuiKey_Delete] = VK_DELETE;
			m_pIO->KeyMap[ImGuiKey_Backspace] = VK_BACK;
			m_pIO->KeyMap[ImGuiKey_Space] = VK_SPACE;
			m_pIO->KeyMap[ImGuiKey_Enter] = VK_RETURN;
			m_pIO->KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
			m_pIO->KeyMap[ImGuiKey_KeyPadEnter] = VK_RETURN;
			m_pIO->KeyMap[ImGuiKey_A] = 'A';
			m_pIO->KeyMap[ImGuiKey_C] = 'C';
			m_pIO->KeyMap[ImGuiKey_V] = 'V';
			m_pIO->KeyMap[ImGuiKey_X] = 'X';
			m_pIO->KeyMap[ImGuiKey_Y] = 'Y';
			m_pIO->KeyMap[ImGuiKey_Z] = 'Z';
		}

		Direct3D12Context& RenderContext = Renderer::GetAs<Direct3D12Context>();
		void* pNativeWindow = RenderContext.GetWindowRef().GetNativeWindow();
		
		// Setup Platform/Renderer bindings
#if defined (IE_PLATFORM_BUILD_WIN32)
		bool impleWin32Succeeded = ImGui_ImplWin32_Init(pNativeWindow);
		if (!impleWin32Succeeded)
			IE_DEBUG_LOG(LogSeverity::Error, "Failed to initialize ImGui for Win32 - D3D 12. Some controls may not be functional or editor may not be rendered.");
#endif // IE_PLATFORM_BUILD_WIN32
		HRESULT hr;
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 1;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		hr = RenderContext.GetDeviceContext().CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pDescriptorHeap));

		bool impleDX12Succeeded = ImGui_ImplDX12_Init(
			&RenderContext.GetDeviceContext(),
			RenderContext.GetFrameBufferCount(),
			RenderContext.GetSwapChainBackBufferFormat(),
			m_pDescriptorHeap,
			m_pDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			m_pDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		if (!impleDX12Succeeded)
			IE_DEBUG_LOG(LogSeverity::Warning, "Failed to initialize ImGui for DX12. Editor will not be rendered");

		m_pCommandList = &RenderContext.GetPostProcessPassCommandList();
#endif
	}

	void D3D12ImGuiLayer::OnDetach()
	{
#if defined (IE_PLATFORM_BUILD_WIN32)
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
#endif // IE_PLATFORM_BUILD_WIN32
		Super::OnDetach();
	}

	void D3D12ImGuiLayer::OnImGuiRender()
	{
		Super::OnImGuiRender();

	}

	void D3D12ImGuiLayer::Begin()
	{
#if defined (IE_PLATFORM_BUILD_WIN32)
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
#endif // IE_PLATFORM_BUILD_WIN32
		Super::Begin();
	}

	void D3D12ImGuiLayer::End()
	{
#if defined (IE_PLATFORM_BUILD_WIN32)
		m_pCommandList->SetDescriptorHeaps(1, &m_pDescriptorHeap);
		Super::End();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_pCommandList);
#endif
	}

}
