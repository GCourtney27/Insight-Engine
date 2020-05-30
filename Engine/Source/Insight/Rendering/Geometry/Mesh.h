#pragma once

#include <Insight/Core.h>

#include "Insight/Rendering/Geometry/Vertex.h"
#include "Insight/Math/Transform.h"
#include "Insight/Core/Scene/Scene_Node.h"
#include "Platform/DirectX_Shared/Constant_Buffer_Types.h"
#include "Insight/Rendering/Material.h"

namespace Insight {

	class INSIGHT_API Mesh
	{
	public:
		typedef std::vector<Vertex3D> Verticies;
		typedef std::vector<DWORD> Indices;
	public:
		Mesh(Verticies verticies, Indices indices, Material material);
		Mesh() {}
		~Mesh();

		void Init(Verticies verticies, Indices indices);
		void InitializeLocalVariables();
		void PreRender(const XMMATRIX& parentMat);
		void Render();
		void Destroy();

		inline Transform& GetTransformRef() { return m_Transform; }
		inline const Transform& GetTransform() const { return m_Transform; }
		CB_VS_PerObject GetConstantBuffer();
		
		const UINT& GetNumVertices() const { return m_NumVerticies; }
		const UINT& GetNumIndices()  const { return m_NumIndices; }
		const int& GetVertexBufferSize() const { return m_VBufferSize; }
		const int& GetIndexBufferSize() const { return m_IBufferSize; }

	private:
		void SetupMesh();
		bool InitializeVertexDataForD3D12();
		bool InitializeInstanceBufferD3D12();
		bool InitializeIndexDataForD3D12();
	private:

		// D3D12 Resources
		ID3D12Resource*				m_pVertexBufferUploadHeap = 0;
		ID3D12Resource*				m_pVertexBuffer = 0;
		D3D12_VERTEX_BUFFER_VIEW	m_VertexBufferView = {};

		ID3D12Resource*				m_pIndexBufferUploadHeap = 0;
		ID3D12Resource*				m_pIndexBuffer = 0;
		D3D12_INDEX_BUFFER_VIEW		m_IndexBufferView = {};
		
		ID3D12Resource*				m_pInstanceBufferUploadHeap = 0;
		ID3D12Resource*				m_pInstanceBuffer = 0;
		D3D12_VERTEX_BUFFER_VIEW	m_InstanceBufferView = {};


		ID3D12Device*				m_pLogicalDevice = 0;
		ID3D12GraphicsCommandList*	m_pCommandList = 0;

		UINT						m_NumVerticies = 0;
		UINT						m_NumIndices = 0;
		UINT						m_VBufferSize = 0;
		UINT						m_IBufferSize = 0;
		std::vector<Vertex3D>		m_Verticies;
		std::vector<DWORD>			m_Indices;

		Transform					m_Transform;
		Material					m_Material;
		CB_VS_PerObject				m_ConstantBufferPerObject = {};

	};
}
