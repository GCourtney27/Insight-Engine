#pragma once

#include "Insight/Core/Interfaces.h"

/*
	Represents a base for a graphics context the application will use for rendering.
	Context is API independent, meaning all rendering calls will pass through a *Imple
	method and dispatched to the appropriate active rendering API.

	Example usage:
	Set a vertex buffers for rendering - RenderingContext::SetVertexBuffers(0, 1, m_VSBuffer);
*/

#define RETURN_IF_WINDOW_NOT_VISIBLE if (!m_WindowVisible){ return;}

namespace Insight {



	class ASkySphere;
	class ASkyLight;
	class APostFx;

	class ADirectionalLight;
	class APointLight;
	class ASpotLight;

	class ACamera;

	class Renderer
	{
	public:
		enum class eRenderingAPI
		{
			INVALID,
			D3D_11,
			D3D_12,
		};

		typedef void* VertexBuffer;
		typedef void* IndexBuffer;
	public:
		virtual ~Renderer();

		static Renderer& Get() { return *s_Instance; }
		// Set the target graphics rendering API and create a context to it.
		// Once set, it cannot be changed through the lifespan application, you must 
		// set it re-launch the app.
		static bool SetAPIAndCreateContext(eRenderingAPI ContextType);

		// Initilize renderer's API library.
		static inline bool Init() { return s_Instance->InitImpl(); }
		// Destroy the current graphics context
		static inline void Destroy() { s_Instance->DestroyImpl(); }
		// Submit initilize commands to the GPU.
		static inline bool PostInit() { return s_Instance->PostInitImpl(); }
		// Upload per-frame constants to the GPU as well as lighting information.
		static inline void OnUpdate(const float DeltaMs) { s_Instance->OnUpdateImpl(DeltaMs); }
		// Flush the command allocators and clear render targets.
		static inline void OnPreFrameRender() { s_Instance->OnPreFrameRenderImpl(); }
		// Draws shadow pass first then binds geometry pass for future draw commands.
		static inline void OnRender() { s_Instance->OnRenderImpl(); }
		// Binds light pass.
		static inline void OnMidFrameRender() { s_Instance->OnMidFrameRenderImpl(); }
		// executes the command queue on the GPU. Waits for the GPU to finish before proceeding.
		static inline void ExecuteDraw() { s_Instance->ExecuteDrawImpl(); }
		// Swap buffers with the new frame.
		static inline void SwapBuffers() { s_Instance->SwapBuffersImpl(); }
		// Resize render target, depth stencil and sreen rects when window size is changed.
		static inline void OnWindowResize() { s_Instance->OnWindowResizeImpl(); }
		// Tells the swapchain to enable full screen rendering.
		static inline void OnWindowFullScreen() { s_Instance->OnWindowFullScreenImpl(); }

		static void SetRenderPass(eRenderPass RenderPass) { s_Instance->m_RenderPass = RenderPass; }

		static void SetVertexBuffers(uint32_t StartSlot, uint32_t NumBuffers, VertexBuffer Buffer) { s_Instance->SetVertexBuffersImpl(StartSlot, NumBuffers, Buffer); }
		static void SetIndexBuffer(IndexBuffer Buffer) { s_Instance->SetIndexBufferImpl(Buffer); }
		static void DrawIndexedInstanced(uint32_t IndexCountPerInstance, uint32_t NumInstances, uint32_t StartIndexLocation, uint32_t BaseVertexLoaction, uint32_t StartInstanceLocation) { s_Instance->DrawIndexedInstancedImpl(IndexCountPerInstance, NumInstances, StartIndexLocation, BaseVertexLoaction, StartInstanceLocation); }

		inline static eRenderingAPI GetAPI() { return s_Instance->m_CurrentAPI; }
		inline static uint8_t GetFrameBufferCount() { return s_Instance->m_FrameBufferCount; }
		inline static void SetVSyncEnabled(bool enabled) { s_Instance->m_VSyncEnabled = enabled; }
		inline static void SetWindowWidthAndHeight(UINT width, UINT height, bool isMinimized) 
		{ 
			s_Instance->m_WindowWidth = width;
			s_Instance->m_WindowHeight = height;
			s_Instance->m_IsMinimized = isMinimized;
			s_Instance->m_AspectRatio = static_cast<float>(s_Instance->m_WindowWidth) / static_cast<float>(s_Instance->m_WindowHeight);
			s_Instance->OnWindowResize();
		}

		// Add a Directional Light to the scene. 
		void RegisterDirectionalLight(ADirectionalLight* DirectionalLight) { m_DirectionalLights.push_back(DirectionalLight); }
		void UnRegisterDirectionalLight(ADirectionalLight* DirectionalLight);
		// Add a Point Light to the scene. 
		void RegisterPointLight(APointLight* PointLight) { m_PointLights.push_back(PointLight); }
		void UnRegisterPointLight(APointLight* PointLight);
		// Add a Spot Light to the scene. 
		void RegisterSpotLight(ASpotLight* SpotLight) { m_SpotLights.push_back(SpotLight); }
		void UnRegisterSpotLight(ASpotLight* SpotLight);

		// Add Sky Sphere to the scene. There can never be more than one in the scene at any given time.
		void AddSkySphere(ASkySphere* skySphere) { if (!m_pSkySphere) { m_pSkySphere = skySphere; } }
		// Add a post-fx volume to the scene.
		void AddPostFxActor(APostFx* postFxActor) { {m_pPostFx = postFxActor; } }
		// Add Sky light to the scene for Image-Based Lighting. There can never be more than one 
		// in the scene at any given time.
		void AddSkyLight(ASkyLight* skyLight) { if (!m_SkyLight) { m_SkyLight = skyLight; } }


	private:
		virtual bool InitImpl() = 0;
		virtual void DestroyImpl() = 0;
		virtual bool PostInitImpl() = 0;
		virtual void OnUpdateImpl(const float DeltaMs) = 0;
		virtual void OnPreFrameRenderImpl() = 0;
		virtual void OnRenderImpl() = 0;
		virtual void OnMidFrameRenderImpl() = 0;
		virtual void ExecuteDrawImpl() = 0;
		virtual void SwapBuffersImpl() = 0;
		virtual void OnWindowResizeImpl() = 0;
		virtual void OnWindowFullScreenImpl() = 0;

		virtual void SetVertexBuffersImpl(uint32_t StartSlot, uint32_t NumBuffers, VertexBuffer Buffer) = 0;
		virtual void SetIndexBufferImpl(IndexBuffer Buffer) = 0;
		virtual void DrawIndexedInstancedImpl(uint32_t IndexCountPerInstance, uint32_t NumInstances, uint32_t StartIndexLocation, uint32_t BaseVertexLoaction, uint32_t StartInstanceLocation) = 0;

	protected:
		Renderer(UINT windowWidth, UINT windowHeight, bool vSyncEabled);
	protected:
		eRenderingAPI m_CurrentAPI = eRenderingAPI::INVALID;
		eRenderPass m_RenderPass = eRenderPass::RenderPass_Scene;

		static const uint8_t m_FrameBufferCount = 3u;
		UINT m_WindowWidth;
		UINT m_WindowHeight;

		float m_AspectRatio = 0.0f;
		bool m_VSyncEnabled = false;
		bool m_IsMinimized = false;
		bool m_FullScreenMode = false;
		bool m_WindowedMode = true;
		bool m_WindowVisible = true;
		
		bool m_AllowTearing = true;

		std::vector<APointLight*> m_PointLights;
		std::vector<ADirectionalLight*> m_DirectionalLights;
		std::vector<ASpotLight*> m_SpotLights;

		ASkySphere* m_pSkySphere = nullptr;
		ASkyLight* m_SkyLight = nullptr;
		APostFx* m_pPostFx = nullptr;

		ACamera* m_pWorldCamera = nullptr;

	private:
		static Renderer* s_Instance;
	};

}
