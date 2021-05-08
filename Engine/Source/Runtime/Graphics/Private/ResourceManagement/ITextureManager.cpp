#include <Engine_pch.h>

#include "Runtime/Graphics/Public/ResourceManagement/ITextureManager.h"

namespace Insight
{
	namespace Graphics
	{
		void IManagedTexture::WaitForLoad() const
		{
			while ((volatile bool&)m_IsLoading)
			{
				std::this_thread::yield();
			}
		}

		void IManagedTexture::Unload()
		{
			g_pTextureManager->DestroyTexture(m_MapKey);
		}
		
		void ITextureManager::DestroyDefaultTextures()
		{
			for (UInt32 i = 0; i < DT_NumDefaultTextures; ++i)
			{
				// Release the resources but dont destroy.
				g_DefaultTextures[i]->Destroy();
				g_DefaultTextures[i] = NULL;
			}
		}


		ITextureRef::ITextureRef(const ITextureRef& ref) : m_ref(ref.m_ref)
		{
			if (m_ref != nullptr)
				++m_ref->m_ReferenceCount;
		}

		ITextureRef::ITextureRef(IManagedTexture* tex) : m_ref(tex)
		{
			if (m_ref != nullptr)
				++m_ref->m_ReferenceCount;
		}

		ITextureRef::~ITextureRef()
		{
			if (m_ref != nullptr && --m_ref->m_ReferenceCount == 0)
				m_ref->Unload();
		}

		void ITextureRef::operator= (std::nullptr_t)
		{
			if (m_ref != nullptr)
				--m_ref->m_ReferenceCount;

			m_ref = nullptr;
		}

		void ITextureRef::operator= (ITextureRef& rhs)
		{
			if (m_ref != nullptr)
				--m_ref->m_ReferenceCount;

			m_ref = rhs.m_ref;

			if (m_ref != nullptr)
				++m_ref->m_ReferenceCount;
		}

		bool ITextureRef::IsValid() const
		{
			return m_ref && m_ref->IsValid();
		}

		const ITexture* ITextureRef::Get() const
		{
			return DCast<ITexture*>(m_ref);
		}

		const ITexture* ITextureRef::operator->() const
		{
			IE_ASSERT(m_ref != nullptr);
			return DCast<ITexture*>(m_ref);
		}
	}
}
