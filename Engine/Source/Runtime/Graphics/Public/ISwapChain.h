#pragma once

#include "EngineDefines.h"
#include <Runtime/Graphics/Public/GraphicsCore.h>

#include "Graphics/Public/Resource/IColorBuffer.h"

namespace Insight
{
	namespace Graphics
	{
		class IColorBuffer;
		class IDevice;


		class INSIGHT_API ISwapChain
		{
			friend class IRenderContext;
		public:
			virtual void Initialize(IDevice* pDevice) = 0;

			virtual void SwapBuffers() = 0;
			virtual void Resize(const FVector2& NewResolution) = 0;
			virtual void SetNumBackBuffes(UInt32 NumBuffers) = 0;
			virtual void SetBackBufferFormat(EFormat& Format) = 0;

			virtual void* GetNativeSwapChain() const = 0;

			virtual void ToggleFullScreen(bool IsEnabled) { SetIsFullScreenEnabled(IsEnabled); }
			virtual void ToggleVsync(bool IsEnabled) { SetIsVSyncEnabled(IsEnabled); }

			FORCEINLINE void MoveToNextFrame();

			//
			// Getters
			//
			FORCEINLINE IESwapChainDescription GetDesc()		const { return m_Desc; }
			FORCEINLINE UInt32 GetCurrentFrameIndex()	const { return m_FrameIndex; }
			FORCEINLINE Color	GetClearColor()			const { return m_ClearColor; }
			FORCEINLINE bool GetIsVSyncEnabled()		const { return m_bVSyncEnabled; }
			FORCEINLINE bool GetIsTearingSupported()	const { return m_bAllowTearing; }
			FORCEINLINE UInt32 GetNumBackBuffers()		const { return m_Desc.BufferCount; }
			FORCEINLINE bool GetIsFullScreenEnabled()	const { return m_bFullScreenEnabled; }
			FORCEINLINE IColorBuffer* GetColorBufferForCurrentFrame() const { return m_DisplayPlanes[GetCurrentFrameIndex()]; }

			//
			// Setters
			//
			FORCEINLINE void SetCurrentFrameIndex(UInt32 Index) { m_FrameIndex = Index; }
			FORCEINLINE void SetClearColor(const Color& NewColor);
			FORCEINLINE void SetIsFullScreenEnabled(bool bEnabled) { m_bFullScreenEnabled = bEnabled; }
			FORCEINLINE void SetIsVSyncEnabled(bool bEnabled)
			{
				if (m_bFullScreenEnabled)
					m_bVSyncEnabled = bEnabled;
				else
					IE_LOG(Error, TEXT("Trying to enable VSync while in windowed mode! This is not allowed."));
			}
			FORCEINLINE void SetIsTearingSupported(bool bSupported) { m_bAllowTearing = bSupported; }

		protected:
			ISwapChain()
				: m_FrameIndex(0)
				, m_ClearColor(0.f, 0.f, 0.f)
				, m_bFullScreenEnabled(false)
				, m_bVSyncEnabled(false)
				, m_bAllowTearing(false)
				, m_pDeviceRef(NULL)
				, m_DisplayPlanes()
			{
			}

			virtual ~ISwapChain()
			{
				m_pDeviceRef = NULL;

				for (UInt64 i = 0; i < m_DisplayPlanes.size(); ++i)
					delete m_DisplayPlanes[i];
			}

			//
			// Virtual Methods
			// 
			virtual void UnInitialize() = 0;

		protected:
			IESwapChainDescription m_Desc;
			UInt32 m_FrameIndex;
			Color m_ClearColor;
			bool m_bFullScreenEnabled;
			bool m_bVSyncEnabled;
			bool m_bAllowTearing;

			IDevice* m_pDeviceRef;
			std::vector<IColorBuffer*> m_DisplayPlanes;
		};


		// 
		// Inline Function Implementations
		//

		void ISwapChain::MoveToNextFrame()
		{
			SetCurrentFrameIndex((GetCurrentFrameIndex() + 1) % GetNumBackBuffers());
		}

		void ISwapChain::SetClearColor(const Color& NewColor)
		{
			m_ClearColor = NewColor;
			for (UInt32 i = 0; i < m_DisplayPlanes.size(); ++i)
				m_DisplayPlanes[i]->SetClearColor(m_ClearColor);
		}

	}
}
