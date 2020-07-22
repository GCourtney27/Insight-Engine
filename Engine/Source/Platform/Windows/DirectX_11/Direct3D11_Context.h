#pragma once

#include <Insight/Core.h>

#include "Insight/Rendering/Rendering_Context.h"
#include "Platform/Windows/Error/COM_Exception.h"

#include "Platform/Windows/DirectX_Shared/Constant_Buffer_Types.h"


using Microsoft::WRL::ComPtr;

namespace Insight {

	class WindowsWindow;
	class GeometryManager;

	class ASkySphere;
	class ASkyLight;
	class APostFx;

	class ADirectionalLight;
	class APointLight;
	class ASpotLight;

	class ACamera;

	class INSIGHT_API Direct3D11Context : public RenderingContext
	{
	public:
		Direct3D11Context(WindowsWindow* windowHandle);
		virtual ~Direct3D11Context();

		// Initilize Direc3D 12 library.
		virtual bool InitImpl() override;
		// Destroy the current graphics context
		virtual void DestroyImpl() override;
		// Submit initilize commands to the GPU.
		virtual bool PostInitImpl() override;
		// Upload per-frame constants to the GPU as well as lighting information.
		virtual void OnUpdateImpl(const float deltaTime) override;
		// Flush the command allocators and clear render targets.
		virtual void OnPreFrameRenderImpl() override;
		// Draws shadow pass first then binds geometry pass for future draw commands.
		virtual void OnRenderImpl() override;
		// Binds light pass.
		virtual void OnMidFrameRenderImpl() override;
		// executes the command queue on the GPU. Waits for the GPU to finish before proceeding.
		virtual void ExecuteDrawImpl() override;
		// Swap buffers with the new frame.
		virtual void SwapBuffersImpl() override;
		// Resize render target, depth stencil and sreen rects when window size is changed.
		virtual void OnWindowResizeImpl() override;
		// Tells the swapchain to enable full screen rendering.
		virtual void OnWindowFullScreenImpl() override;

		virtual void SetVertexBuffersImpl(uint32_t StartSlot, uint32_t NumBuffers, VertexBuffer Buffer) override;
		virtual void SetIndexBufferImpl(IndexBuffer Buffer) override;
		virtual void DrawIndexedInstancedImpl(uint32_t IndexCountPerInstance, uint32_t NumInstances, uint32_t StartIndexLocation, uint32_t BaseVertexLoaction, uint32_t StartInstanceLocation) override;

	private:
		HWND*				m_pWindowHandle = nullptr;
		WindowsWindow*		m_pWindow = nullptr;
		GeometryManager*	m_pModelManager = nullptr;
		ACamera*			m_pWorldCamera = nullptr;

		bool				m_WindowResizeComplete = true;

	};

}