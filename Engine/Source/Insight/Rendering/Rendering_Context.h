#pragma once

/*
	Represents a base for a graphics context the application will use for rendering.
*/
#define RETURN_IF_WINDOW_NOT_VISIBLE if (!m_WindowVisible){ return;}

namespace Insight {


	class RenderingContext
	{
	public:
		enum class RenderingAPI
		{
			D3D_11,
			D3D_12,
		};

		typedef void* VertexBuffer;
		typedef void* IndexBuffer;
	public:
		virtual ~RenderingContext();

		static RenderingContext& Get() { return *s_Instance; }
		static bool SetContext(RenderingAPI ContextType);

		static bool Init() { return s_Instance->InitImpl(); }
		static void Destroy() { s_Instance->DestroyImpl(); }
		static bool PostInit() { return s_Instance->PostInitImpl(); }
		static void OnUpdate(const float DeltaMs) { s_Instance->OnUpdateImpl(DeltaMs); }
		static void OnPreFrameRender() { s_Instance->OnPreFrameRenderImpl(); }
		static void OnRender() { s_Instance->OnRenderImpl(); }
		static void OnMidFrameRender() { s_Instance->OnMidFrameRenderImpl(); }
		static void ExecuteDraw() { s_Instance->ExecuteDrawImpl(); }
		static void SwapBuffers() { s_Instance->SwapBuffersImpl(); }
		static void OnWindowResize() { s_Instance->OnWindowResizeImpl(); }
		static void OnWindowFullScreen() { s_Instance->OnWindowFullScreenImpl(); }

		static void SetVertexBuffers(uint32_t StartSlot, uint32_t NumBuffers, VertexBuffer Buffer) { s_Instance->SetVertexBuffersImpl(StartSlot, NumBuffers, Buffer); }
		static void SetIndexBuffers(IndexBuffer Buffer) { s_Instance->SetIndexBuffersImpl(Buffer); }
		static void DrawIndexedInstanced(uint32_t IndexCountPerInstance, uint32_t NumInstances, uint32_t StartIndexLocation, uint32_t BaseVertexLoaction, uint32_t StartInstanceLocation) { s_Instance->DrawIndexedInstancedImpl(IndexCountPerInstance, NumInstances, StartIndexLocation, BaseVertexLoaction, StartInstanceLocation); }

		inline static RenderingAPI GetAPI() { return s_Instance->m_CurrentAPI; }
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
		virtual void SetIndexBuffersImpl(IndexBuffer Buffer) = 0;
		virtual void DrawIndexedInstancedImpl(uint32_t IndexCountPerInstance, uint32_t NumInstances, uint32_t StartIndexLocation, uint32_t BaseVertexLoaction, uint32_t StartInstanceLocation) = 0;

	protected:
		RenderingContext(UINT windowWidth, UINT windowHeight, bool vSyncEabled);
	protected:
		RenderingAPI m_CurrentAPI;
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

		
	private:
		static RenderingContext* s_Instance;
	};

}
