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

		// Temporary should eventually use Insight keu codes
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
		void* pWindow = Application::Get().GetWindow().GetNativeWindow();
		bool succeeded1 = ImGui_ImplWin32_Init( ((HWND*)pWindow) );

		bool succeeded = ImGui_ImplDX12_Init(&graphicsContext->GetDevice(), 
			graphicsContext->GetFrameBufferCount(),
			DXGI_FORMAT_R8G8B8A8_UNORM,
			&graphicsContext->GetImGuiDescriptorHeap(),
			graphicsContext->GetImGuiDescriptorHeap().GetCPUDescriptorHandleForHeapStart(),
			graphicsContext->GetImGuiDescriptorHeap().GetGPUDescriptorHandleForHeapStart());

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

		//ImGuiIO& io = ImGui::GetIO();
		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		RenderingContext* renderContext = &Application::Get().GetWindow().GetRenderContext();
		Direct3D12Context* graphicsContext = reinterpret_cast<Direct3D12Context*>(renderContext);

		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), &graphicsContext->GetCommandList());
	}

	void ImGuiLayer::OnEvent(Event & event)
	{
	}

}

