#include <Engine_pch.h>

#include "D3D11_ImGui_Layer.h"

#include "Insight/Core/Application.h"

#include "Platform/DirectX_11/Direct3D11_Context.h"
#include "Platform/Win32/Win32_Window.h"

#include "imgui.h"
#include "examples/imgui_impl_dx11.h"
#include "examples/imgui_impl_win32.h"
#include <examples/imgui_impl_dx11.cpp>


namespace Insight {



	void D3D11ImGuiLayer::OnAttach()
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

		Direct3D11Context& RenderContext = Renderer::GetAs<Direct3D11Context>();
		
		HWND& WindowHandle = RenderContext.GetWindowRefAs<Win32Window>().GetWindowHandleRef();

		if (!ImGui_ImplWin32_Init(WindowHandle)) {
			IE_DEBUG_LOG(LogSeverity::Warning, "Failed to initialize ImGui for Win32 - D3D 12. Some controls may not be functional or editor may not be rendered.");
		}
		ImGui_ImplDX11_Init(&RenderContext.GetDevice(), &RenderContext.GetDeviceContext());

	}

	void D3D11ImGuiLayer::OnDetach()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGuiLayer::OnDetach();
	}

	void D3D11ImGuiLayer::OnImGuiRender()
	{
		ImGuiLayer::OnImGuiRender();

	}

	void D3D11ImGuiLayer::Begin()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGuiLayer::Begin();
	}

	void D3D11ImGuiLayer::End()
	{
		ImGuiLayer::End();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

}
