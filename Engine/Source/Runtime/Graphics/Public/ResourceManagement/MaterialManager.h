#pragma once

#include "Runtime/Core.h"

#include "Runtime/Graphics/Public/ieMaterial.h"

namespace Insight
{
	class INSIGHT_API ManagedMaterial : public ieMaterial
	{
		friend class MaterialRef;
		friend class MaterialManager;
	public:
		ManagedMaterial() = default;
		virtual ~ManagedMaterial() = default;

		void WaitForLoad() const;

	protected:
		ManagedMaterial(const FString& HashName)
			: m_MapKey(HashName)
			, m_IsValid(false)
			, m_IsLoading(true)
			, m_ReferenceCount(1)
		{
		}
		void SetLoadCompleted(bool Completed)
		{
			m_IsLoading = !Completed;
		}

	protected:
		bool IsValid(void) const { return m_IsValid; }
		void Unload();

		FString m_MapKey; // For deleting from the map later.
		bool m_IsValid;
		bool m_IsLoading;
		UInt64 m_ReferenceCount;
	};

	class INSIGHT_API MaterialRef
	{
	public:
		MaterialRef(const MaterialRef& ref);
		MaterialRef(ManagedMaterial* pTex = nullptr);
		~MaterialRef();
		MaterialRef& operator=(const MaterialRef& Other)
		{
			this->m_Ref = Other.m_Ref;
			return *this;
		}


		void operator= (std::nullptr_t);
		void operator= (MaterialRef& rhs);

		// Check that this points to a valid texture (which loaded successfully)
		bool IsValid() const;

		// Get the texture pointer.  Client is responsible to not dereference
		// null pointers.
		ieMaterial* Get();

		ieMaterial* operator->();

	private:
		ManagedMaterial* m_Ref;
	};



	class INSIGHT_API MaterialManager
	{
	public:
		MaterialManager() = default;
		~MaterialManager() = default;

		void LoadMaterialFromFile(const FString& Path);

		void DestroyMaterial(const FString& Key);

		MaterialRef GetMaterialByName(const FString& Id)
		{
			return m_MaterialCache.at(Id).get();
		}

	private:
		std::map< FString, std::unique_ptr<ManagedMaterial> > m_MaterialCache;

		std::mutex m_MapMutex;
	};
}
