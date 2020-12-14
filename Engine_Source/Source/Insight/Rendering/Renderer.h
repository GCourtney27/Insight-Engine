#pragma once

#include <Insight/Core.h>

#include "Insight/Core/Interfaces.h"

#include "Insight/Rendering/ASky_Sphere.h"
#include "Insight/Rendering/Geometry/Vertex_Buffer.h"
#include "Insight/Rendering/Geometry/Index_Buffer.h"

#include "Platform/DirectX_Shared/Constant_Buffer_Types.h"
#include "Insight/Events/Event.h"

/*
	Represents a base for a graphics context the application will use for rendering.
	Context is API independent, meaning all rendering calls will pass through a *Imple
	method and dispatched to the appropriate active rendering API.

	Example usage:
	Set a vertex buffers for rendering - Renderer::SetVertexBuffers(0, 1, ieVertexBuffer);
*/

#define RETURN_IF_WINDOW_NOT_VISIBLE if (!m_WindowVisible || !m_WindowResizeComplete) { return; }


namespace Insight {


	class ASkyLight;
	class APostFx;

	class ADirectionalLight;
	class APointLight;
	class ASpotLight;

	class PointLightComponent;

	class Window;

	namespace Runtime {
		class ACamera;
	}

	class INSIGHT_API Renderer
	{
	public:

		/*
			Valid render API's supported by the engine.
		*/
		typedef enum class TargetRenderAPI
		{
			Invalid,
			Direct3D_11,
			Direct3D_12,
		} TargetRenderAPI;

		/*
			Settings that can be adjusted to tune graphics perfoance and compatability.
		*/
		struct GraphicsSettings
		{
			TargetRenderAPI TargetRenderAPI = TargetRenderAPI::Direct3D_12;
			uint32_t MaxAnisotropy = 1U;	// Texture Filtering (1, 4, 8, 16) *16 highest quality
			float MipLodBias = 0.0f;		// Texture Quality (0 - 9) *0 highest quality
			bool RayTraceEnabled = false;
			int pad[3];
		};

	public:
		virtual ~Renderer();

		/*
			Returns a reference to the renderer as a given type.
		*/
		template <class RenderContext>
		static inline RenderContext& GetAs()
		{
			// Make sure the requested class is a valid render context.
			constexpr bool ValidClass = std::is_base_of<Renderer, RenderContext>::value;
			static_assert(ValidClass, "Trying to get Rendere with invalid context type.");

			return *((RenderContext*)s_Instance);
		}

		/* 
			Set the target graphics rendering API and create a context to it.
			Once set, it cannot be changed through the lifespan application, you must 
			set it re-launch the app.
			@param GraphicsSettings - A reference to the graphics settings the renderer will use. Usually loaded from a chched file.
			@param pWindow - A refrence to the application's main window the renderer will output to.
		*/
		static bool SetSettingsAndCreateContext(const GraphicsSettings& GraphicsSettings, std::shared_ptr<Window> pWindow);

		/*
			Initilize renderer's API library.
		*/
		static inline bool Init() { return s_Instance->Init_Impl(); }

		/*
			Destroy the current graphics context
		*/
		static inline void Destroy() { s_Instance->Destroy_Impl(); }
		
		/*
			Submit initilize commands to the GPU.
		*/
		static inline bool PostInit() { return s_Instance->PostInit_Impl(); }
		
		/*
			Upload per-frame constants to the GPU as well as lighting information.
			@param DeltaMs - The change in time from frame to frame.
		*/
		static inline void OnUpdate(const float DeltaMs) 
		{
			// Process any events that eed to take place before the start of this frame.
			s_Instance->HandleEvents();
			// Then update.
			s_Instance->OnUpdate_Impl(DeltaMs); 
		}

		/*
			Flush the command allocators and clear render targets.
		*/
		static inline void OnPreFrameRender() { s_Instance->OnPreFrameRender_Impl(); }
		
		/*
			Draws shadow pass first then binds geometry pass for future draw commands.
		*/
		static inline void OnRender() { s_Instance->OnRender_Impl(); }
		
		/*
			Executes operations that happen in the middle of a frame.
		*/
		static inline void OnMidFrameRender() { s_Instance->OnMidFrameRender_Impl(); }
		
		/*
			Executes the command queue on the GPU. Waits for the GPU to finish before proceeding
			to the next frame.
		*/
		static inline void ExecuteDraw() { s_Instance->ExecuteDraw_Impl(); }
		
		/*
			Swap buffers with the new frame.
		*/
		static inline void SwapBuffers() { s_Instance->SwapBuffers_Impl(); }
		
		/*
			Returns the window reference as a specified type.
		*/
		template <class WindowClassType>
		static inline WindowClassType& GetWindowRefAs() 
		{ 
			constexpr bool IsValidWindow = std::is_base_of<Window, Win32Window>::value;
			static_assert(IsValidWindow, "Class type is not a valid window.");
			return *(WindowClassType*)(s_Instance->m_pWindowRef.get());
		}
		
		/*
			Returns the window reference this render context outputs to.
		*/
		static inline Window& GetWindowRef() { return *(s_Instance->m_pWindowRef); }

		/*
			Set the graphics settings for the context
		*/
		static void SetGraphicsSettings(const GraphicsSettings& GraphicsSettings) { s_Instance->m_GraphicsSettings = GraphicsSettings; }





		//-----------------
		// Event Handers   |
		//-----------------

		/*
			Resize render target, depth stencil and sreen rects when window size is changed.
		*/
		inline bool OnWindowResize(WindowResizeEvent& e) 
		{
			s_Instance->m_IsMinimized = e.GetIsMinimized();
			s_Instance->OnWindowResize_Impl();
			return true;
		}

		/*
			Tells the swapchain to enable full screen rendering.
		*/
		inline bool OnWindowFullScreen(WindowToggleFullScreenEvent& e) { s_Instance->OnWindowFullScreen_Impl(); return true; }
		
		/*
			Reloads all shaders
		*/
		inline bool OnShaderReload(ShaderReloadEvent& e) { s_Instance->OnShaderReload_Impl(); return true; }
		
		/* 
			Push an event to the renderers queue. Window resize events, shader resload events etc. 
			Before each fram the renderer ill handle all events in the queue before proessing a frame, 
			eliminateing the possibility of the Game thread modifying resources the Render thread is using mid frame.
		*/
		template <class EventType>
		static void PushEvent(EventType& e)
		{ 
			if constexpr (std::is_same<EventType, WindowResizeEvent>::value)
			{
				s_Instance->m_WindowResizeEventQueue.push(e);
			}
			if constexpr (std::is_same<EventType, WindowToggleFullScreenEvent>::value)
			{
				s_Instance->m_WindowFullScreenEventQueue.push(e);
			}
			if constexpr (std::is_same<EventType, ShaderReloadEvent>::value)
			{
				s_Instance->m_ShaderReloadEventQueue.push(e);
			}

		}

		/*
			Called when the editor begins to render.
		*/
		static inline void OnEditorRender() { s_Instance->OnEditorRender_Impl(); }

		/*
			Get the current graphics settings the renderer is using.
		*/
		static GraphicsSettings GetGraphicsSettings() { return s_Instance->m_GraphicsSettings; }
		
		/*
			Sets the currently active render pass.
			@param RenderPass - The type of render pass to bind to.
		*/
		static void SetRenderPass(RenderPassType RenderPass) { s_Instance->m_RenderPass = RenderPass; }
		
		/*
			Binds a vertex buffer to the input assembler for drawing.
		*/
		static void SetVertexBuffers(uint32_t StartSlot, uint32_t NumBuffers, ieVertexBuffer* pBuffers) { s_Instance->SetVertexBuffers_Impl(StartSlot, NumBuffers, pBuffers); }

		/*
			Binds a index buffer to the input assembler for drawing.
		*/
		static void SetIndexBuffer(ieIndexBuffer* pBuffer) { s_Instance->SetIndexBuffer_Impl(pBuffer); }
		
		/*
			Draws a peice of geometry with optional indexing paramerters.
		*/
		static void DrawIndexedInstanced(uint32_t IndexCountPerInstance, uint32_t NumInstances, uint32_t StartIndexLocation, uint32_t BaseVertexLoaction, uint32_t StartInstanceLocation) { s_Instance->DrawIndexedInstanced_Impl(IndexCountPerInstance, NumInstances, StartIndexLocation, BaseVertexLoaction, StartInstanceLocation); }
		
		/*
			TODO: Renders text to be rendered.
			@param Text - Text to be rendered on screen
		*/
		static void DrawText(const char* Text) { s_Instance->DrawText_Impl(Text); }
		
		/*
			Renders the world sky sphere.
		*/
		static void RenderSkySphere() { s_Instance->RenderSkySphere_Impl(); }
		
		/*
			Creates a Skybox to be rendered in the world.
		*/
		static bool CreateSkybox() { return s_Instance->CreateSkybox_Impl(); }

		/*
			Destroys the world sky sphere and any resources it contains
		*/
		static void DestroySkybox() { s_Instance->DestroySkybox_Impl(); }

		/*
			Returns true if ray tracing is enabled. False if not.
		*/
		inline static bool GetIsRayTraceEnabled() { return s_Instance->m_GraphicsSettings.RayTraceEnabled; }
		
		/*
			Returns the currently active render API in used for this graphics context.
		*/
		inline static TargetRenderAPI GetAPI() { return s_Instance->m_GraphicsSettings.TargetRenderAPI; }
		
		/*
			Returns the amount of frame buffers the swap chain contains.
		*/
		inline static uint8_t GetFrameBufferCount() { return s_Instance->m_FrameBufferCount; }

		/*
			Sets the camera to render the world with.
		*/
		static void SetActiveCamera(Runtime::ACamera* pCamera) { s_Instance->m_pWorldCameraRef = pCamera; }

		/*
			Returns the contant buffr for the world directional light.
		*/
		CB_PS_DirectionalLight GetDirectionalLightCB() const;

		/*
			Add a Directional Light to the scene. 
		*/
		static void RegisterWorldDirectionalLight(ADirectionalLight* pDirectionalLight) { s_Instance->m_pWorldDirectionalLight = pDirectionalLight; }
		
		/*
			Remove a Directional Light from the scene
		*/
		static void UnRegisterWorldDirectionalLight();
		
		/*
			Add a Point Light to the scene. 
		*/
		static void RegisterPointLight(APointLight* pPointLight) { s_Instance->m_PointLights.push_back(pPointLight); }
		
		/*
			Remove a Point Light from the scene
		*/
		static void UnRegisterPointLight(APointLight* pPointLight);
		
		/*
			Add a Spot Light to the scene. 
		*/
		static void RegisterSpotLight(ASpotLight* pSpotLight) { s_Instance->m_SpotLights.push_back(pSpotLight); }
		
		/*
			Remove a Spot Light from the scene
		*/
		static void UnRegisterSpotLight(ASpotLight* pSpotLight);

		/*
			Add Sky Sphere to the scene. There can never be more than one in the scene at any given time.
		*/
		static void RegisterSkySphere(ASkySphere* SkySphere) { if (!s_Instance->m_pSkySphere) { s_Instance->m_pSkySphere = SkySphere; } }
		
		/*
			Removes the world sky sphere from the scene.
		*/
		static void UnRegisterSkySphere() { if (s_Instance->m_pSkySphere) { delete s_Instance->m_pSkySphere; } }
		
		/*
			Add a post-fx volume to the scene.
		*/
		static void AddPostFxActor(APostFx* PostFxActor) { s_Instance->m_pPostFx = PostFxActor;  }
		
		/* 
			Add Sky light to the scene for Image-Based Lighting. There can never be more than one 
			in the scene at any given time.
		*/
		static void AddSkyLight(ASkyLight* SkyLight) { if (!s_Instance->m_pSkyLight) { s_Instance->m_pSkyLight = SkyLight; } }

	protected:
		virtual bool Init_Impl() = 0;
		virtual void Destroy_Impl() = 0;
		virtual bool PostInit_Impl() = 0;
		virtual void OnUpdate_Impl(const float DeltaMs) = 0;
		virtual void OnPreFrameRender_Impl() = 0;
		virtual void OnRender_Impl() = 0;
		virtual void OnMidFrameRender_Impl() = 0;
		virtual void OnEditorRender_Impl() = 0;
		virtual void ExecuteDraw_Impl() = 0;
		virtual void SwapBuffers_Impl() = 0;
		virtual void OnWindowResize_Impl() = 0;
		virtual void OnWindowFullScreen_Impl() = 0;
		virtual void OnShaderReload_Impl() = 0;

		virtual void SetVertexBuffers_Impl(uint32_t StartSlot, uint32_t NumBuffers, ieVertexBuffer* pBuffers) = 0;
		virtual void SetIndexBuffer_Impl(ieIndexBuffer* pBuffer) = 0;
		virtual void DrawIndexedInstanced_Impl(uint32_t IndexCountPerInstance, uint32_t NumInstances, uint32_t StartIndexLocation, uint32_t BaseVertexLoaction, uint32_t StartInstanceLocation) = 0;
		virtual void DrawText_Impl(const char* Text) = 0;

		virtual void RenderSkySphere_Impl() = 0;
		virtual bool CreateSkybox_Impl() = 0;
		virtual void DestroySkybox_Impl() = 0;

	protected:

		/*
			The main render method. Renders the world and all geometry associated with it.
		*/
		Renderer();

		/*
			Set wether or not hardware accelerated ray tracing is supported. Should 
			be called once at app initialization time.
			@param Supprted - True if ray tracing is supported, false if not.
		*/
		void SetIsRayTraceSupported(bool Supported) { m_IsRayTraceSupported = Supported; }

		/*
			Handles all events in the graphics queue before proceeding to render the next frame.
		*/
		void HandleEvents();

	protected:
		RenderPassType m_RenderPass = RenderPassType::RenderPassType_Scene;
		GraphicsSettings m_GraphicsSettings = {};

		static const uint8_t m_FrameBufferCount = 3u;

		bool m_IsMinimized = false;
		bool m_FullScreenMode = false;
		bool m_WindowedMode = true;
		bool m_WindowVisible = true;
		
		bool m_AllowTearing = true;
		bool m_IsRayTraceSupported = false; // Assume real-time ray tracing is not supported on the GPU.

		std::vector<APointLight*> m_PointLights;
		ADirectionalLight* m_pWorldDirectionalLight;
		std::vector<ASpotLight*> m_SpotLights;

		ASkySphere* m_pSkySphere = nullptr;
		ASkyLight* m_pSkyLight = nullptr;
		APostFx* m_pPostFx = nullptr;

		Runtime::ACamera* m_pWorldCameraRef = nullptr;
		std::shared_ptr<Window> m_pWindowRef;

		std::queue<WindowResizeEvent> m_WindowResizeEventQueue;
		std::queue<WindowToggleFullScreenEvent> m_WindowFullScreenEventQueue;
		std::queue<ShaderReloadEvent> m_ShaderReloadEventQueue;

	private:
		static Renderer* s_Instance;
	};

}
