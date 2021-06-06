#pragma once

#include "EngineDefines.h"

#include "Graphics/Geometry/Vertex.h"

namespace Insight {

	

	using Verticies = std::vector<Vertex3D>;

	class INSIGHT_API ieVertexBuffer
	{
	public:
		ieVertexBuffer(Verticies Verticies)
			:	m_Verticies(std::move(Verticies))
		{
		}
		ieVertexBuffer() = default;
		virtual ~ieVertexBuffer() {}

		virtual void Destroy() {}

		uint32_t GetNumVerticies()	{ return m_NumVerticies; }
		uint32_t GetBufferSize()	{ return m_BufferSize; }
	protected:
		virtual bool CreateResources() { return true; }

	protected:
		Verticies	m_Verticies;
		uint32_t	m_NumVerticies = 0u;
		uint32_t	m_BufferSize = 0u;
	};

}