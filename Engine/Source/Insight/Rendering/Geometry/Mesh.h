#pragma once

#include "Insight/Core.h"

#include "Insight/Rendering/Geometry/Vertex.h"
#include "Insight/Math/Transform.h"
#include "Platform/DirectX_Shared/Constant_Buffer_Types.h"

namespace Insight {

	class Mesh 
	{
	public:
		Mesh(std::vector<Vertex> verticies, std::vector<DWORD> indices);
		~Mesh();

		inline const Transform& GetTransform() const { return m_Transform; }
		CB_VS_PerObject GetConstantBuffer();
		
		const UINT& GetNumVertices() const { return m_NumVerticies; }
		const UINT& GetNumIndices()  const { return m_NumIndices; }
		const int& GetVertexBufferSize() const { return m_VBufferSize; }
		const int& GetIndexBufferSize() const { return m_IBufferSize; }

		void Draw();

		void Destroy();

		void SetupMesh();
	private:
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

		UINT					m_NumVerticies = 0;
		UINT					m_NumIndices = 0;
		int						m_VBufferSize = 0;
		int						m_IBufferSize = 0;
		std::vector<Vertex>		m_Verticies;
		std::vector<DWORD>		m_Indices;

		Transform m_Transform;
		CB_VS_PerObject m_ConstantBufferPerObject = {};

		//std::vector<Texture> m_Textures;
	};
}
