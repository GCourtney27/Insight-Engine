#include <Engine_pch.h>

#include "Runtime/Graphics/Public/ResourceManagement/MaterialManager.h"

namespace Insight
{

	void ManagedMaterial::WaitForLoad() const
	{
		while ((volatile bool&)m_IsLoading)
			std::this_thread::yield();
	}

	void ManagedMaterial::Unload()
	{
		Graphics::g_MaterialManager.DestroyMaterial(m_MapKey);
	}

	MaterialRef::MaterialRef(const MaterialRef& ref)
		: m_Ref(ref.m_Ref)
	{
		if (m_Ref != nullptr)
			++m_Ref->m_ReferenceCount;
	}

	MaterialRef::MaterialRef(ManagedMaterial* tex)
		: m_Ref(tex)
	{
		if (m_Ref != nullptr)
			++m_Ref->m_ReferenceCount;
	}

	MaterialRef::~MaterialRef()
	{
		if (m_Ref != nullptr && --m_Ref->m_ReferenceCount == 0)
			m_Ref->Unload();
	}

	void MaterialRef::operator= (std::nullptr_t)
	{
		if (m_Ref != nullptr)
			--m_Ref->m_ReferenceCount;

		m_Ref = nullptr;
	}

	void MaterialRef::operator= (MaterialRef& rhs)
	{
		if (m_Ref != nullptr)
			--m_Ref->m_ReferenceCount;

		m_Ref = rhs.m_Ref;

		if (m_Ref != nullptr)
			++m_Ref->m_ReferenceCount;
	}

	bool MaterialRef::IsValid() const
	{
		return m_Ref && m_Ref->IsValid();
	}

	ieMaterial* MaterialRef::Get()
	{
		return (ieMaterial*)m_Ref;
	}

	ieMaterial* MaterialRef::operator->()
	{
		IE_ASSERT(m_Ref != nullptr);
		return (ieMaterial*)m_Ref;
	}


	//
	// Material Manager Implementation
	//

	void MaterialManager::LoadMaterialFromFile(const FString& Path)
	{
		ManagedMaterial* pMat = NULL;

		{
			std::lock_guard<std::mutex> Gaurd(m_MapMutex);

			FString Name = StringHelper::GetFilenameFromDirectory(Path);
			UInt64 HashName = std::hash<FString>{}(Name);

			auto Iter = m_MaterialCache.find(Name);
			if (Iter != m_MaterialCache.end())
			{
				pMat = Iter->second.get();
				pMat->WaitForLoad();
				return;
			}
			else
			{
				pMat = new ManagedMaterial(Name);
				// TODO load from file here.
				m_MaterialCache[Name].reset(pMat);
			}

			pMat->SetUID(HashName);
			pMat->Initialize();
			pMat->SetLoadCompleted(true);
		}
	}

	void MaterialManager::DestroyMaterial(const FString& Key)
	{
		auto Iter = m_MaterialCache.find(Key);
		if (Iter != m_MaterialCache.end())
		{
			(*Iter).second.get()->UnInitialize();
			m_MaterialCache.erase(Iter);
		}
	}
}
