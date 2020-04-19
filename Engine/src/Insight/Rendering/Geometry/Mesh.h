#pragma once

#include <Insight/Core.h>

#include "Insight/Rendering/Geometry/Vertex.h"


namespace Insight {

	class Mesh
	{
	public:
		Mesh(std::vector<Vertex> verticies, std::vector<UINT> indices);
		~Mesh();

		const UINT& GetNumVertices() const { return m_NumVerticies; }
		const UINT& GetNumIndices()  const { return m_NumIndices; }
		const int& GetVertexBufferSize() const { return m_VBufferSize; }
		const int& GetIndexBufferSize() const { return m_IBufferSize; }

		void Draw();
		void Destroy();

		void SetupMesh();
	private:
		bool InitializeVertexDataForD3D12();
		bool InitializeIndexDataForD3D12();
	private:

		//D3D 12 Resources
		ID3D12Resource*				m_pVertexBuffer;
		ID3D12Resource*				m_pVertexBufferUploadHeap;
		D3D12_VERTEX_BUFFER_VIEW	m_VertexBufferView = {};

		ID3D12Resource*				m_pIndexBuffer;
		ID3D12Resource*				m_pIndexBufferUploadHeap;
		D3D12_INDEX_BUFFER_VIEW		m_IndexBufferView = {};
		
		ID3D12Device5*				m_pLogicalDevice;
		ID3D12GraphicsCommandList*	m_pCommandList;

		UINT					m_NumVerticies = 0;
		UINT					m_NumIndices = 0;
		int						m_VBufferSize = 0;
		int						m_IBufferSize = 0;
		std::vector<Vertex>		m_Verticies;
		std::vector<UINT>		m_Indices;

		bool m_IncludeInGBufferPass = true;
		//std::vector<Texture> m_Textures;
	};
}
