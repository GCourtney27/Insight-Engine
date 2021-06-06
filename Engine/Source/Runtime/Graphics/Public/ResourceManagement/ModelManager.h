#pragma once

#include "EngineDefines.h"

#include "Graphics/Public/WorldRenderer/StaticMeshGeometry.h"


namespace ofbx
{
	struct Mesh;
}

namespace Insight
{
	class INSIGHT_API ManagedStaticMeshGeometry : public StaticMeshGeometry
	{
		friend class StaticMeshGeometryRef;
		friend class StaticGeometryManager;
	public:
		ManagedStaticMeshGeometry() = default;
		virtual ~ManagedStaticMeshGeometry() = default;

		void WaitForLoad() const;

	protected:
		ManagedStaticMeshGeometry(const FString& HashName)
			: m_MapKey(HashName)
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


	class INSIGHT_API StaticMeshGeometryRef
	{
	public:
		StaticMeshGeometryRef(const StaticMeshGeometryRef& ref);
		StaticMeshGeometryRef(ManagedStaticMeshGeometry* tex = nullptr);
		~StaticMeshGeometryRef();
		StaticMeshGeometryRef& operator=(const StaticMeshGeometryRef& Other)
		{
			this->m_Ref = Other.m_Ref;
			return *this;
		}


		void operator= (std::nullptr_t);
		void operator= (StaticMeshGeometryRef& rhs);

		// Check that this points to a valid peice of static geometry (which loaded successfully)
		bool IsValid() const;

		// Get the texture pointer.  Client is responsible to not dereference
		// null pointers.
		StaticMeshGeometry* Get();

		StaticMeshGeometry* operator->();

	private:
		ManagedStaticMeshGeometry* m_Ref;
	};



	class INSIGHT_API StaticGeometryManager
	{
	public:
		StaticGeometryManager() = default;
		~StaticGeometryManager() = default;

		void LoadFBXFromFile(const FString& FilePath);
		void DestroyMesh(const FString& Key);
		bool MeshExists(const FString& Name);

		StaticMeshGeometryRef GetStaticMeshByName(const FString& Name)
		{
			return m_ModelCache.at(Name).get();
		}

		StaticMeshGeometryRef RegisterGeometry(const FString& Name, void* Verticies, UInt32 VertexDataSizeInBytes, UInt32 NumVerticies, UInt32 VertexSize, void* Indices, UInt32 IndexDataSizeInBytes, UInt32 NumIndices);

	protected:
		void ParseFBXMesh(const ofbx::Mesh& Mesh, std::vector<GeometryVertex3D>& OutVerticies, std::vector<UInt32>& OutIndices);

	private:
		std::map< FString, std::unique_ptr<ManagedStaticMeshGeometry> > m_ModelCache;
		std::mutex m_MapMutex;
	};
}
