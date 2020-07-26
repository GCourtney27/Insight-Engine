#pragma once

#include <Insight/Core.h>

#include "Insight/Core/Interfaces.h"
#include "Insight/Rendering/Geometry/Vertex_Buffer.h"
#include "Insight/Rendering/Geometry/Index_Buffer.h"

/*
	Represents a base for a graphics context the application will use for rendering.
	Context is API independent, meaning all rendering calls will pass through a *Imple
	method and dispatched to the appropriate active rendering API.

	Example usage:
	Set a vertex buffers for rendering - Renderer::SetVertexBuffers(0, 1, ieVertexBuffer);
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

	class INSIGHT_API Renderer
	{
	public:
		enum eTargetRenderAPI
		{
			INVALID = 0,
			D3D_11 = 1,
			D3D_12 = 2,
		};

		struct GraphicsSettings
		{
			eTargetRenderAPI TargetRenderAPI = eTargetRenderAPI::D3D_11;
			uint32_t MaxAnisotropy = 1U; // Texture Filtering (1, 4, 8, 16)
			float MipLodBias = 0.0f; // Texture Quality (0 - 9)
		};

	public:
		virtual ~Renderer();

		static Renderer& Get() { return *s_Instance; }

		// Set the target graphics rendering API and create a context to it.
		// Once set, it cannot be changed through the lifespan application, you must 
		// set it re-launch the app.
		static bool SetSettingsAndCreateContext(GraphicsSettings GraphicsSettings);
		// Set the graphics settings for the context
		static void SetGraphicsSettings(GraphicsSettings GraphicsSettings) { s_Instance->m_GraphicsSettings = GraphicsSettings; }

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

		static void SetVertexBuffers(uint32_t StartSlot, uint32_t NumBuffers, ieVertexBuffer* pBuffers) { s_Instance->SetVertexBuffersImpl(StartSlot, NumBuffers, pBuffers); }
		static void SetIndexBuffer(ieIndexBuffer* pBuffer) { s_Instance->SetIndexBufferImpl(pBuffer); }
		static void DrawIndexedInstanced(uint32_t IndexCountPerInstance, uint32_t NumInstances, uint32_t StartIndexLocation, uint32_t BaseVertexLoaction, uint32_t StartInstanceLocation) { s_Instance->DrawIndexedInstancedImpl(IndexCountPerInstance, NumInstances, StartIndexLocation, BaseVertexLoaction, StartInstanceLocation); }

		static void RenderSkySphere() { s_Instance->RenderSkySphereImpl(); }
		static bool CreateSkybox() { return s_Instance->CreateSkyboxImpl(); }
		static void DestroySkybox() { s_Instance->DestroySkyboxImpl(); }


		inline static eTargetRenderAPI GetAPI() { return s_Instance->m_GraphicsSettings.TargetRenderAPI; }
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
		static void RegisterDirectionalLight(ADirectionalLight* DirectionalLight) { s_Instance->m_DirectionalLights.push_back(DirectionalLight); }
		// Remove a Directional Light from the scene
		static void UnRegisterDirectionalLight(ADirectionalLight* DirectionalLight);
		// Add a Point Light to the scene. 
		static void RegisterPointLight(APointLight* PointLight) { s_Instance->m_PointLights.push_back(PointLight); }
		// Remove a Point Light from the scene
		static void UnRegisterPointLight(APointLight* PointLight);
		// Add a Spot Light to the scene. 
		static void RegisterSpotLight(ASpotLight* SpotLight) { s_Instance->m_SpotLights.push_back(SpotLight); }
		// Remove a Spot Light from the scene
		static void UnRegisterSpotLight(ASpotLight* SpotLight);

		// Add Sky Sphere to the scene. There can never be more than one in the scene at any given time.
		static void AddSkySphere(ASkySphere* skySphere) { if (!s_Instance->m_pSkySphere) { s_Instance->m_pSkySphere = skySphere; } }
		// Add a post-fx volume to the scene.
		static void AddPostFxActor(APostFx* postFxActor) { s_Instance->m_pPostFx = postFxActor;  }
		// Add Sky light to the scene for Image-Based Lighting. There can never be more than one 
		// in the scene at any given time.
		static void AddSkyLight(ASkyLight* skyLight) { if (!s_Instance->m_SkyLight) { s_Instance->m_SkyLight = skyLight; } }

	protected:
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

		virtual void SetVertexBuffersImpl(uint32_t StartSlot, uint32_t NumBuffers, ieVertexBuffer* pBuffers) = 0;
		virtual void SetIndexBufferImpl(ieIndexBuffer* pBuffer) = 0;
		virtual void DrawIndexedInstancedImpl(uint32_t IndexCountPerInstance, uint32_t NumInstances, uint32_t StartIndexLocation, uint32_t BaseVertexLoaction, uint32_t StartInstanceLocation) = 0;

		virtual void RenderSkySphereImpl() = 0;
		virtual bool CreateSkyboxImpl() = 0;
		virtual void DestroySkyboxImpl() = 0;

	protected:
		Renderer(uint32_t windowWidth, uint32_t windowHeight, bool vSyncEabled);
	protected:
		eRenderPass m_RenderPass = eRenderPass::RenderPass_Scene;
		GraphicsSettings m_GraphicsSettings = {}; // TODO Load this from a settings file

		static const uint8_t m_FrameBufferCount = 3u;
		uint32_t m_WindowWidth;
		uint32_t m_WindowHeight;

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
