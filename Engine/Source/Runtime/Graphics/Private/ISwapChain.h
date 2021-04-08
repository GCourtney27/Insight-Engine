#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/Resource/IColorBuffer.h"
#include "Runtime/Graphics/Public/CommonEnums.h"
#include "Runtime/Graphics/Public/CommonStructs.h"

namespace Insight
{
	namespace Graphics
	{
		class IColorBuffer;
		class IDevice;

		struct IESwapChainDesc
		{
			UInt32 Width;
			UInt32 Height;
			UInt32 BufferCount;
			SampleDesc SampleDesc;
			ETextureFormat Format;
			void* NativeWindow;
		};

		class INSIGHT_API ISwapChain
		{
			friend class IRenderContext;
		public:
			virtual void Initialize(IDevice* pDevice) = 0;

			virtual void SwapBuffers() = 0;
			virtual void Resize(const FVector2& NewResolution) = 0;
			virtual void SetNumBackBuffes(UInt32 NumBuffers) = 0;
			virtual void SetBackBufferFormat(ETextureFormat& Format) = 0;

			virtual void* GetNativeSwapChain() const = 0;

			virtual void ToggleFullScreen(bool IsEnabled) { SetIsFullScreenEnabled(IsEnabled); }
			virtual void ToggleVsync(bool IsEnabled) { SetIsVSyncEnabled(IsEnabled); }

			FORCE_INLINE void MoveToNextFrame();

			//
			// Getters
			//
			FORCE_INLINE IESwapChainDesc GetDesc()		const { return m_Desc; }
			FORCE_INLINE UInt32 GetCurrentFrameIndex()	const { return m_FrameIndex; }
			FORCE_INLINE Color	GetClearColor()			const { return m_ClearColor; }
			FORCE_INLINE bool GetIsVSyncEnabled()		const { return m_bVSyncEnabled; }
			FORCE_INLINE bool GetIsTearingSupported()	const { return m_bAllowTearing; }
			FORCE_INLINE UInt32 GetNumBackBuffers()		const { return m_Desc.BufferCount; }
			FORCE_INLINE bool GetIsFullScreenEnabled()	const { return m_bFullScreenEnabled; }
			FORCE_INLINE IColorBuffer* GetColorBufferForCurrentFrame() const { return m_DisplayPlanes[GetCurrentFrameIndex()]; }

			//
			// Setters
			//
			FORCE_INLINE void SetCurrentFrameIndex(UInt32 Index) { m_FrameIndex = Index; }
			FORCE_INLINE void SetClearColor(const Color& NewColor);
			FORCE_INLINE void SetIsFullScreenEnabled(bool bEnabled) { m_bFullScreenEnabled = bEnabled; }
			FORCE_INLINE void SetIsVSyncEnabled(bool bEnabled)
			{
				if (m_bFullScreenEnabled)
					m_bVSyncEnabled = bEnabled;
				else
					IE_LOG(Error, TEXT("Trying to enable VSync while in windowed mode! This is not allowed."));
			}
			FORCE_INLINE void SetIsTearingSupported(bool bSupported) { m_bAllowTearing = bSupported; }

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
			IESwapChainDesc m_Desc;
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
