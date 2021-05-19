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


		ITextureRef::ITextureRef(const ITextureRef& ref) 
			: m_Ref(ref.m_Ref)
		{
			if (m_Ref != nullptr)
				++m_Ref->m_ReferenceCount;
		}

		ITextureRef::ITextureRef(IManagedTexture* tex) : m_Ref(tex)
		{
			if (m_Ref != nullptr)
				++m_Ref->m_ReferenceCount;
		}

		ITextureRef::~ITextureRef()
		{
			if (m_Ref != nullptr && --m_Ref->m_ReferenceCount == 0)
				m_Ref->Unload();
		}

		void ITextureRef::operator= (std::nullptr_t)
		{
			if (m_Ref != nullptr)
				--m_Ref->m_ReferenceCount;

			m_Ref = nullptr;
		}

		void ITextureRef::operator= (ITextureRef& rhs)
		{
			if (m_Ref != nullptr)
				--m_Ref->m_ReferenceCount;

			m_Ref = rhs.m_Ref;

			if (m_Ref != nullptr)
				++m_Ref->m_ReferenceCount;
		}

		bool ITextureRef::IsValid() const
		{
			return m_Ref && m_Ref->IsValid();
		}

		const ITexture* ITextureRef::Get() const
		{
			return DCast<ITexture*>(m_Ref);
		}

		const ITexture* ITextureRef::operator->() const
		{
			IE_ASSERT(m_Ref != nullptr);
			return DCast<ITexture*>(m_Ref);
		}
	}
}
