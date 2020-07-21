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

		virtual bool Init() = 0;
		virtual bool PostInit() = 0;
		virtual void OnUpdate(const float& deltaTime) = 0;
		virtual void OnPreFrameRender() = 0;
		virtual void OnRender() = 0;
		virtual void OnMidFrameRender() = 0;
		virtual void ExecuteDraw() = 0;
		virtual void SwapBuffers() = 0;
		virtual void OnWindowResize() = 0;
		virtual void OnWindowFullScreen() = 0;

		virtual bool SetVertexBuffers(uint32_t StartSlot, uint32_t NumBuffers, VertexBuffer Buffer) = 0;
		virtual bool SetIndexBuffers(IndexBuffer Buffer) = 0;
		virtual bool DrawIndexedInstanced(uint32_t IndexCountPerInstance, uint32_t NumInstances, uint32_t StartIndexLocation, uint32_t BaseVertexLoaction, uint32_t StartInstanceLocation) = 0;

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
