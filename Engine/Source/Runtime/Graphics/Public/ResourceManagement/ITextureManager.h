#pragma once

#include <Runtime/Core.h>
#include <Runtime/Graphics/Public/GraphicsCore.h>

#include "Runtime/Graphics/Public/Resource/ITexture.h"

namespace Insight
{
	namespace Graphics
	{

		class INSIGHT_API IManagedTexture
		{
			friend class ITextureRef;

		public:
			void WaitForLoad() const;
			virtual void CreateFromMemory(DataBlob memory, EDefaultTexture fallback, bool sRGB) = 0;

		protected:
			IManagedTexture(const FString& Path)
				: m_MapKey(Path)
			{
			}

		protected:
			bool IsValid(void) const { return m_IsValid; }
			void Unload();

			FString m_MapKey;		// For deleting from the map later
			bool m_IsValid;
			bool m_IsLoading;
			size_t m_ReferenceCount;
		};

		class INSIGHT_API ITextureManager
		{
			friend class IRenderContextFactory;
			friend class D3D12RenderContextFactory;
			friend class IRenderContext;
		public:

			virtual ITextureRef LoadTexture(const FString& FileName, EDefaultTexture Fallback, bool forceSRGB) = 0;
			virtual void DestroyTexture(const FString& Key) = 0;

		private:
			virtual IManagedTexture* FindOrLoadTexture(const FString& FileName, EDefaultTexture Fallback, bool forceSRGB) = 0;

			void DestroyDefaultTextures();

		protected:
			ITextureManager() = default;
			virtual ~ITextureManager() = default;

			virtual void Initialize() = 0;
			virtual void UnInitialize() = 0;

			
			std::mutex m_Mutex;
		};


		//
		// A handle to a ManagedTexture.  Constructors and destructors modify the reference
		// count.  When the last reference is destroyed, the TextureManager is informed that
		// the texture should be deleted.
		//
		class ITextureRef
		{
		public:

			ITextureRef(const ITextureRef& ref);
			ITextureRef(IManagedTexture* tex = nullptr);
			~ITextureRef();
			ITextureRef& operator=(const ITextureRef& Other)
			{
				this->m_Ref = Other.m_Ref;
				return *this;
			}
			const FString& GetCacheKey() const
			{
				return m_Ref->m_MapKey;
			}

			void operator= (std::nullptr_t);
			void operator= (ITextureRef& rhs);

			// Check that this points to a valid texture (which loaded successfully)
			bool IsValid() const;

			// Get the texture pointer.  Client is responsible to not dereference
			// null pointers.
			const ITexture* Get() const;

			const ITexture* operator->() const;

		private:
			IManagedTexture* m_Ref;
		};
	}
}

