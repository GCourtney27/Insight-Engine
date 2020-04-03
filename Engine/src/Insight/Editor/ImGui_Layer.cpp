#include "ie_pch.h"

#include "ImGui_Layer.h"
#include "Insight/Application.h"
#include "Platform/DirectX12/Direct3D12_Context.h"
#include "Platform/Windows/Windows_Window.h"

#include "imgui.h"
#include "Platform/ImGui/ImGui_DX12_Renderer.h"
#include "Platform/ImGui/imgui_impl_win32.h"

namespace Insight {


	ImGuiLayer::ImGuiLayer()
		: Layer("ImGui Layer")
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		// Temporary should eventually use Insight key codes
		io.KeyMap[ImGuiKey_Tab] = VK_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
		io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
		io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
		io.KeyMap[ImGuiKey_Home] = VK_HOME;
		io.KeyMap[ImGuiKey_End] = VK_END;
		io.KeyMap[ImGuiKey_Insert] = VK_INSERT;
		io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
		io.KeyMap[ImGuiKey_Space] = VK_SPACE;
		io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
		io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
		io.KeyMap[ImGuiKey_KeyPadEnter] = VK_RETURN;
		io.KeyMap[ImGuiKey_A] = 'A';
		io.KeyMap[ImGuiKey_C] = 'C';
		io.KeyMap[ImGuiKey_V] = 'V';
		io.KeyMap[ImGuiKey_X] = 'X';
		io.KeyMap[ImGuiKey_Y] = 'Y';
		io.KeyMap[ImGuiKey_Z] = 'Z';

		RenderingContext* renderContext = &Application::Get().GetWindow().GetRenderContext();
		Direct3D12Context* graphicsContext = reinterpret_cast<Direct3D12Context*>(renderContext);
		HWND* pWindowHandle = static_cast<HWND*>(Application::Get().GetWindow().GetNativeWindow());
		bool impleWin32Succeeded = ImGui_ImplWin32_Init( (pWindowHandle) );
		if(!impleWin32Succeeded)
			IE_CORE_WARN("Failed to initialize ImGui for Win32. Some controls may not be functional or editor may not be rendered.");

		bool impleDX12Succeeded = ImGui_ImplDX12_Init(&graphicsContext->GetDevice(), 
														graphicsContext->GetFrameBufferCount(),
														DXGI_FORMAT_R8G8B8A8_UNORM,
														&graphicsContext->GetImGuiDescriptorHeap(),
														graphicsContext->GetImGuiDescriptorHeap().GetCPUDescriptorHandleForHeapStart(),
														graphicsContext->GetImGuiDescriptorHeap().GetGPUDescriptorHandleForHeapStart());
		if(!impleDX12Succeeded)
			IE_CORE_WARN("Failed to initialize ImGui for DX12. Editor will not be rendered");

	}

	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnUpdate()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		RenderingContext* renderContext = &Application::Get().GetWindow().GetRenderContext();
		Direct3D12Context* graphicsContext = reinterpret_cast<Direct3D12Context*>(renderContext);

		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), &graphicsContext->GetCommandList());
	}

	void ImGuiLayer::OnEvent(Event & event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<MouseButtonPressedEvent>(IE_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonPressedEvent));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(IE_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonReleasedEvent));

		dispatcher.Dispatch<MouseMovedEvent>(IE_BIND_EVENT_FN(ImGuiLayer::OnMouseMovedEvent));
		dispatcher.Dispatch<MouseScrolledEvent>(IE_BIND_EVENT_FN(ImGuiLayer::OnMouseScrollEvent));
		
		dispatcher.Dispatch<KeyPressedEvent>(IE_BIND_EVENT_FN(ImGuiLayer::OnKeyPressedEvent));
		dispatcher.Dispatch<KeyReleasedEvent>(IE_BIND_EVENT_FN(ImGuiLayer::OnKeyReleasedEvent));
		
		dispatcher.Dispatch<KeyTypedEvent>(IE_BIND_EVENT_FN(ImGuiLayer::OnKeyTypedEvent));
		
		dispatcher.Dispatch<WindowResizeEvent>(IE_BIND_EVENT_FN(ImGuiLayer::OnWindowResizedEvent));

	}

	bool ImGuiLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.GetMouseButton()] = true;

		return false;
	}

	bool ImGuiLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.GetMouseButton()] = false;

		return false;
	}

	bool ImGuiLayer::OnMouseMovedEvent(MouseMovedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(e.GetX(), e.GetY());

		return false;
	}

	bool ImGuiLayer::OnMouseScrollEvent(MouseScrolledEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheel += e.GetYOffset();
		io.MouseWheelH += e.GetXOffset();

		return false;
	}

	bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.GetKeyCode()] = true;

		io.KeyCtrl = io.KeysDown[VK_LCONTROL] || io.KeysDown[VK_RCONTROL];
		io.KeyShift = io.KeysDown[VK_LSHIFT] || io.KeysDown[VK_RCONTROL];
		io.KeySuper = io.KeysDown[VK_LWIN] || io.KeysDown[VK_RWIN];
		io.KeyAlt = io.KeysDown[VK_LMENU] || io.KeysDown[VK_RMENU];

		return false;
	}

	bool ImGuiLayer::OnKeyReleasedEvent(KeyReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.GetKeyCode()] = false; 
		
		return false;
	}

	bool ImGuiLayer::OnKeyTypedEvent(KeyTypedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		int keycode = e.GetKeyCode();
		if (keycode > 0 && keycode < 0x10000)
			io.AddInputCharacter((unsigned short)keycode);

		return false;
	}

	bool ImGuiLayer::OnWindowResizedEvent(WindowResizeEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(e.GetWidth(), e.GetHeight());
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

		//window->Resize(e.GetWidth(), e.GetHeight());
		return false;
	}

}

