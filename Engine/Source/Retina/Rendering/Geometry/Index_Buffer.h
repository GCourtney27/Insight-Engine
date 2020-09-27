#pragma once

#include <Retina/Core.h>


namespace Retina {

	typedef std::vector<unsigned long> Indices;

	class RETINA_API ieIndexBuffer
	{
	public:
		ieIndexBuffer(Indices Indices)
			: m_Indices(std::move(Indices)) {}
		virtual ~ieIndexBuffer() = default;

		virtual void Destroy() {}

		uint32_t GetNumIndices() { return m_NumIndices; }
		uint32_t GetBufferSize() { return m_BufferSize; }
	protected:
		virtual bool CreateResources() { return true; }

	protected:
		Indices			m_Indices;
		unsigned long	m_NumIndices = 0;
		uint32_t		m_BufferSize = 0U;
	};

}