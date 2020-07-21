#pragma once

#include <Insight/Core.h>

#include "Insight/Rendering/Geometry/Vertex.h"
#include "Insight/Math/Transform.h"
#include "Insight/Core/Scene/Scene_Node.h"
#include "Platform/Windows/DirectX_Shared/Constant_Buffer_Types.h"
#include "Insight/Rendering/Material.h"

namespace Insight {

	class INSIGHT_API Mesh
	{
	public:
		typedef std::vector<Vertex3D> Verticies;
		typedef std::vector<unsigned long> Indices;
	public:
		Mesh(Verticies verticies, Indices indices);
		Mesh() {}
		Mesh(Mesh&& mesh) noexcept;
		~Mesh();

		void Init(Verticies verticies, Indices indices);
		void InitializeVariables();
		void PreRender(const XMMATRIX& parentMat);
		void Render(ID3D12GraphicsCommandList*& pCommandList);
		void Destroy();
		void OnImGuiRender();

		inline Transform& GetTransformRef() { return m_Transform; }
		inline const Transform& GetTransform() const { return m_Transform; }
		CB_VS_PerObject GetConstantBuffer();
		
		const uint32_t GetNumVertices() const { return m_NumVerticies; }
		const uint32_t GetNumIndices()  const { return m_NumIndices; }
		const uint32_t GetVertexBufferSize() const { return m_VBufferSize; }
		const uint32_t GetIndexBufferSize() const { return m_IBufferSize; }

	private:
		void SetupMesh();
		bool InitializeVertexDataForD3D12();
		bool InitializeIndexDataForD3D12();
	private:

		// D3D12 Resources
		ID3D12Resource*				m_pVertexBufferUploadHeap = 0;
		ID3D12Resource*				m_pVertexBuffer = 0;
		D3D12_VERTEX_BUFFER_VIEW	m_VertexBufferView = {};

		ID3D12Resource*				m_pIndexBufferUploadHeap = 0;
		ID3D12Resource*				m_pIndexBuffer = 0;
		D3D12_INDEX_BUFFER_VIEW		m_IndexBufferView = {};

		ID3D12Device*				m_pDeviceContext = 0;

		uint32_t					m_NumVerticies = 0;
		uint32_t					m_NumIndices = 0;
		uint32_t					m_VBufferSize = 0;
		uint32_t					m_IBufferSize = 0;
		Verticies					m_Verticies;
		Indices						m_Indices;

		Transform					m_Transform;
		CB_VS_PerObject				m_ConstantBufferPerObject = {};

		bool						m_CastsShadows = true;
	};
}
