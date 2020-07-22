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
		//Mesh(Mesh&& mesh) noexcept;
		~Mesh();

		void PreRender(const XMMATRIX& parentMat);
		void Render(ID3D12GraphicsCommandList*& pCommandList);
		void Destroy();
		void OnImGuiRender();

		inline Transform& GetTransformRef() { return m_Transform; }
		inline const Transform& GetTransform() const { return m_Transform; }
		CB_VS_PerObject GetConstantBuffer();

		uint32_t GetNumVertices() { return m_pVertexBuffer->GetNumVerticies(); }
		uint32_t GetVertexBufferSize() { return m_pVertexBuffer->GetBufferSize(); }

		uint32_t GetNumIndices() { return m_pIndexBuffer->GetBufferSize(); }
		uint32_t GetIndexBufferSize() { return m_pIndexBuffer->GetBufferSize(); }

	private:
		void Init(Verticies& verticies, Indices& indices);
		void CreateBuffers(Verticies& Verticies, Indices& Indices);

	private:
		VertexBuffer* m_pVertexBuffer;
		IndexBuffer* m_pIndexBuffer;

		Transform					m_Transform;
		CB_VS_PerObject				m_ConstantBufferPerObject = {};

		bool						m_CastsShadows = true;
	};
}
