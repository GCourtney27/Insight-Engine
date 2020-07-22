#pragma once

#include <Insight/Core.h>


namespace Insight {

	typedef std::vector<unsigned long> Indices;

	class INSIGHT_API IndexBuffer
	{
	public:
		IndexBuffer(Indices Indices)
			: m_Indices(std::move(Indices)) {}
		virtual ~IndexBuffer() = default;

		virtual void Destroy() {}

		uint32_t GetNumIndices() { return m_NumIndices; }
		uint32_t GetBufferSize() { return m_BufferSize; }
	protected:
		virtual bool CreateResources() { return true; }

	protected:
		Indices		m_Indices;
		uint32_t	m_NumIndices = 0U;
		uint32_t	m_BufferSize = 0U;
	};

}