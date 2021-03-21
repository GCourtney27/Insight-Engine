#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/ITexture.h"

namespace Insight
{
	namespace Graphics
	{
		struct IESwapChainCreateDesc
		{
			UInt32 Width;
			UInt32 Height;
			UInt32 BufferCount;
			IESampleDesc SampleDesc;
			IETextureFormat Format;
			bool AllowTearing;
			void* NativeWindow;
		};
		
		class INSIGHT_API ISwapChain
		{
			friend class IRenderContext;
		public:
			
			virtual void SwapBuffers() = 0;
			virtual void Resize() = 0;

			virtual void* GetNativeSwapChain() const = 0;

			virtual void ToggleFullScreen(bool IsEnabled)	{ SetIsFullScreenEnabled(IsEnabled); }
			virtual void ToggleVsync(bool IsEnabled)		{ SetIsVSyncEnabled(IsEnabled); }

			//
			// Getters/Setters
			//
			FORCE_INLINE UInt32 GetCurrentFrameIndex()	const { return m_FrameIndex; }
			FORCE_INLINE Color	GetClearColor()			const { return m_ClearColor; }
			FORCE_INLINE bool GetIsFullScreenEnabled()	const { return m_FullScreenEnabled; }
			FORCE_INLINE bool GetIsVSyncEnabled()		const { return m_VSyncEnabled; }

			FORCE_INLINE void SetCurrentFrameIndex(UInt32 Index)	{ m_FrameIndex = Index; }
			FORCE_INLINE void SetClearColor(const Color& NewColor)	{ m_ClearColor = NewColor; }
			FORCE_INLINE void SetIsFullScreenEnabled(bool Enabled)	{ m_FullScreenEnabled = Enabled; }
			FORCE_INLINE void SetIsVSyncEnabled(bool Enabled)
			{
				if (m_FullScreenEnabled)
					m_VSyncEnabled = Enabled;
				else
					IE_LOG(Error, TEXT("Trying to enable vsync while in windowed mode! This is not allowed."));
			}

		protected:
			ISwapChain() 
				: m_FrameIndex(0)
				, m_ClearColor(0.f, 0.f, 0.f)
				, m_FullScreenEnabled(false)
				, m_VSyncEnabled(false)
			{
			}
			
			virtual ~ISwapChain()
			{
			}
			
			//
			// Virtual Methods
			// 
			
			virtual void UnInitialize() = 0;

		protected:
			UInt32 m_FrameIndex;
			Color m_ClearColor;
			bool m_FullScreenEnabled;
			bool m_VSyncEnabled;
		};
	}
}
