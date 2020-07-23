#pragma once

#include <Insight/Core.h>

#include "Insight/Math/Transform.h"

#include "Platform/Windows/DirectX_Shared/Constant_Buffer_Types.h"

#include "Insight/Rendering/Geometry/Vertex_Buffer.h"
#include "Insight/Rendering/Geometry/Index_Buffer.h"

namespace Insight {


	class INSIGHT_API Mesh
	{
	public:
		Mesh(Verticies Verticies, Indices Indices);
		//Mesh(Mesh&& mesh) noexcept;
		~Mesh();

		void PreRender(const XMMATRIX& parentMat);
		void Render(ID3D12GraphicsCommandList*& pCommandList);
		void Destroy();
		void OnImGuiRender();

		inline ieTransform& GetTransformRef() { return m_Transform; }
		inline const ieTransform& GetTransform() const { return m_Transform; }
		CB_VS_PerObject GetConstantBuffer();

		uint32_t GetVertexCount();
		uint32_t GetVertexBufferSize();

		uint32_t GetIndexCount();
		uint32_t GetIndexBufferSize();

	private:
		void Init(Verticies& verticies, Indices& indices);
		void CreateBuffers(Verticies& Verticies, Indices& Indices);

	private:
		ieVertexBuffer* m_pVertexBuffer;
		ieIndexBuffer* m_pIndexBuffer;

		ieTransform					m_Transform;
		CB_VS_PerObject				m_ConstantBufferPerObject = {};

		bool						m_CastsShadows = true;
	};
}
