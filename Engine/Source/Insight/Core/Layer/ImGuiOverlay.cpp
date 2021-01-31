#include <Engine_pch.h>

#include "ImGuiOverlay.h"
#include "Insight/Core/Application.h"

#if defined (IE_PLATFORM_BUILD_WIN32)
#include "imgui.h"
#endif

namespace Insight {


	ImGuiLayer::ImGuiLayer()
		: Layer("ImGui Layer")
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
#if defined (IE_PLATFORM_BUILD_WIN32)
		delete m_pIO;
#endif
	}

	void ImGuiLayer::OnAttach()
	{
#if defined (IE_PLATFORM_BUILD_WIN32)
		IMGUI_CHECKVERSION();
		m_pIO = new ImGuiIO();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;		// Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();
		
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;

		*m_pIO = io;
#endif
	}

	void ImGuiLayer::OnDetach()
	{
#if defined (IE_PLATFORM_BUILD_WIN32)
		ImGui::DestroyContext();
#endif
	}

	void ImGuiLayer::OnImGuiRender()
	{
	}

	void ImGuiLayer::Begin()
	{
#if defined (IE_PLATFORM_BUILD_WIN32)
		ImGui::NewFrame();
		//ImGuizmo::BeginFrame();
		ImGui::DockSpaceOverViewport(0, ImGuiDockNodeFlags_PassthruCentralNode);
#endif
	}

	void ImGuiLayer::End()
	{
#if defined (IE_PLATFORM_BUILD_WIN32)
		ImGui::Render();
#endif
	}

	void ImGuiLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		// Mouse Buttons
		dispatcher.Dispatch<MouseButtonPressedEvent>(IE_BIND_LOCAL_EVENT_FN(ImGuiLayer::OnMouseButtonPressedEvent));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(IE_BIND_LOCAL_EVENT_FN(ImGuiLayer::OnMouseButtonReleasedEvent));
		// Mouse Moved
		//dispatcher.Dispatch<MouseMovedEvent>(IE_BIND_EVENT_FN(ImGuiLayer::OnMouseMovedEvent));
		dispatcher.Dispatch<MouseRawMoveEvent>(IE_BIND_LOCAL_EVENT_FN(ImGuiLayer::OnMouseRawMoveEvent));
		dispatcher.Dispatch<MouseScrolledEvent>(IE_BIND_LOCAL_EVENT_FN(ImGuiLayer::OnMouseScrollEvent));
		// Key Pressed
		dispatcher.Dispatch<KeyPressedEvent>(IE_BIND_LOCAL_EVENT_FN(ImGuiLayer::OnKeyPressedEvent));
		dispatcher.Dispatch<KeyReleasedEvent>(IE_BIND_LOCAL_EVENT_FN(ImGuiLayer::OnKeyReleasedEvent));
		// Key Typed
		dispatcher.Dispatch<KeyTypedEvent>(IE_BIND_LOCAL_EVENT_FN(ImGuiLayer::OnKeyTypedEvent));
		// Widnow Resized
		dispatcher.Dispatch<WindowResizeEvent>(IE_BIND_LOCAL_EVENT_FN(ImGuiLayer::OnWindowResizedEvent));
	}

	bool ImGuiLayer::IsMouseOverUI()
	{
#if defined (IE_PLATFORM_BUILD_WIN32)
		return ImGui::IsAnyWindowHovered();
#endif
		return false;
	}

	bool ImGuiLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent& e)
	{
#if defined (IE_PLATFORM_BUILD_WIN32)
		ImGuiIO& io = ImGui::GetIO();
		uint8_t Keycode = 0;
		TranslateMouseCode(e.GetKeyCode(), Keycode);
		io.MouseDown[Keycode] = true;
#endif
		return false;
	}

	bool ImGuiLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e)
	{
#if defined (IE_PLATFORM_BUILD_WIN32)
		ImGuiIO& io = ImGui::GetIO();
		uint8_t Keycode = 0;
		TranslateMouseCode(e.GetKeyCode(), Keycode);
		io.MouseDown[Keycode] = false;
#endif
		return false;
	}

	bool ImGuiLayer::OnMouseRawMoveEvent(MouseRawMoveEvent& e)
	{
#if defined (IE_PLATFORM_BUILD_WIN32)
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2((float)e.GetX(), (float)e.GetY());
#endif
		return false;
	}

	bool ImGuiLayer::OnMouseMovedEvent(MouseMovedEvent& e)
	{
#if defined (IE_PLATFORM_BUILD_WIN32)
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(e.GetX(), e.GetY());
#endif
		return false;
	}

	bool ImGuiLayer::OnMouseScrollEvent(MouseScrolledEvent& e)
	{
#if defined (IE_PLATFORM_BUILD_WIN32)
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheel += e.GetYOffset();
		io.MouseWheelH += e.GetXOffset();
#endif
		return false;
	}

	bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent& e)
	{
#if defined (IE_PLATFORM_BUILD_WIN32)
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.GetKeyCode()] = true;
#endif
		return false;
	}

	bool ImGuiLayer::OnKeyReleasedEvent(KeyReleasedEvent& e)
	{
#if defined (IE_PLATFORM_BUILD_WIN32)
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.GetKeyCode()] = false;
#endif
		return false;
	}

	bool ImGuiLayer::OnKeyTypedEvent(KeyTypedEvent& e)
	{
#if defined (IE_PLATFORM_BUILD_WIN32)
		ImGuiIO& io = ImGui::GetIO();
		int keycode = e.GetKeyCode();
		if (keycode > 0 && keycode < 0x10000)
			io.AddInputCharacter((unsigned short)keycode);
#endif
		return false;
	}

	bool ImGuiLayer::OnWindowResizedEvent(WindowResizeEvent& e)
	{
#if defined (IE_PLATFORM_BUILD_WIN32)
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(static_cast<float>(e.GetWidth()), static_cast<float>(e.GetHeight()));
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
#endif
		return false;
	}



}

