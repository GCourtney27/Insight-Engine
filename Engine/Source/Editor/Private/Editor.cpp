#include <Engine_pch.h>

#include "Editor/Public/Editor.h"

#include "Core/Public/Window.h"
#include "Graphics/Public/IDevice.h"
#include "Graphics/Public/IRenderContext.h"

#if IE_WIN32
//#	include "imgui.h"
//#	include <examples/imgui_impl_dx12.cpp>
//#	include "examples/imgui_impl_dx12.h"
//#	include "examples/imgui_impl_win32.h"
#endif

namespace Insight
{
	namespace Editor
	{
		Editor::Editor()
			: m_bIsInitialized(false)
		{
		}

		Editor::~Editor()
		{
		}
		
		void Editor::Initialize(Graphics::IRenderContext* pRenderContext, shared_ptr<Window> pWindow)
		{
			switch (pRenderContext->GetBackendType())
			{
			case Graphics::RB_Direct3D12:
			{
				ID3D12Device* pD3D12Device = RCast<ID3D12Device*>(Graphics::g_pDevice->GetNativeDevice());
				//IE_ASSERT( ImGui_ImplWin32_Init(pWindow->GetNativeWindow()) );


				break;
			}
			/*TODO: case Graphics::RB_Direct3D11:
				break;*/

			default:
				IE_LOG(Error, TEXT("Invalid render context backend type provided when initializing editor."));
				IE_ASSERT(false);
			}


			m_bIsInitialized = false;
		}
	}
}
