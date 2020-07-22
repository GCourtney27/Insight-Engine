#pragma once

#include <Insight/Core.h>

#include "Insight/Math/Transform.h"
#include "Insight/Core/Scene/Scene_Node.h"
#include "Platform/Windows/DirectX_Shared/Constant_Buffer_Types.h"

#include "Insight/Rendering/Material.h"
#include "Insight/Rendering/Geometry/Vertex_Buffer.h"
#include "Insight/Rendering/Geometry/Index_Buffer.h"

namespace Insight {

	class INSIGHT_API Mesh
	{
	public:
		Mesh(Verticies verticies, Indices indices);
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

		uint32_t GetNumVertices() { return m_VertexBuffer.GetNumVerticies(); }
		uint32_t GetVertexBufferSize() { return m_VertexBuffer.GetBufferSize(); }

		uint32_t GetNumIndices() { return m_IndexBuffer.GetBufferSize(); }
		uint32_t GetIndexBufferSize() { return m_IndexBuffer.GetBufferSize(); }

	private:
		void SetupMesh();
		bool InitializeVertexDataForD3D12();
		bool InitializeIndexDataForD3D12();
	private:
		VertexBuffer m_VertexBuffer;
		IndexBuffer m_IndexBuffer;

		Transform					m_Transform;
		CB_VS_PerObject				m_ConstantBufferPerObject = {};

		bool						m_CastsShadows = true;
	};
}
