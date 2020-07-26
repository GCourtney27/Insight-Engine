#include <ie_pch.h>

#include "D3D12_ImGui_Layer.h"

#include "Insight/Core/Application.h"

#include "Platform/Windows/DirectX_12/Direct3D12_Context.h"

#include "imgui.h"
#include "examples/imgui_impl_dx12.h"
#include "examples/imgui_impl_win32.h"


namespace Insight {


	void D3D12ImGuiLayer::OnAttach()
	{
		ImGuiLayer::OnAttach();

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

		Direct3D12Context* graphicsContext = reinterpret_cast<Direct3D12Context*>(&Renderer::Get());

		HWND* pWindowHandle = static_cast<HWND*>(Application::Get().GetWindow().GetNativeWindow());

		// Setup Platform/Renderer bindings
		bool impleWin32Succeeded = ImGui_ImplWin32_Init(pWindowHandle);
		if (!impleWin32Succeeded)
			IE_CORE_WARN("Failed to initialize ImGui for Win32. Some controls may not be functional or editor may not be rendered.");

		HRESULT hr;
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 1;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		hr = graphicsContext->GetDeviceContext().CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pDescriptorHeap));

		bool impleDX12Succeeded = ImGui_ImplDX12_Init(&graphicsContext->GetDeviceContext(),
			graphicsContext->GetFrameBufferCount(),
			DXGI_FORMAT_R8G8B8A8_UNORM,
			m_pDescriptorHeap,
			m_pDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			m_pDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		if (!impleDX12Succeeded)
			IE_CORE_WARN("Failed to initialize ImGui for DX12. Editor will not be rendered");

		m_pCommandList = &graphicsContext->GetScenePassCommandList();
	}

	void D3D12ImGuiLayer::OnDetach()
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGuiLayer::OnDetach();
	}

	void D3D12ImGuiLayer::OnImGuiRender()
	{
		ImGuiLayer::OnImGuiRender();

	}

	void D3D12ImGuiLayer::Begin()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGuiLayer::Begin();
	}

	void D3D12ImGuiLayer::End()
	{
		m_pCommandList->SetDescriptorHeaps(1, &m_pDescriptorHeap);
		ImGuiLayer::End();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_pCommandList);
	}

}
