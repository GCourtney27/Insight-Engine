#pragma once

#include <Insight/Core.h>

#include <vector>

#include "Render_Passes.h"

namespace Insight {


	class RenderPassStack
	{
	public:
		RenderPassStack() = default;
		~RenderPassStack() = default;


		inline void PushPass(RenderPass* pPass)
		{
			m_RenderPasses.emplace(m_RenderPasses.begin() + m_PassInsertIndex, pPass);
			pPass->OnStackAttach();
			m_PassInsertIndex++;
		}

		inline void PushPassOverlay(RenderPass* pOverlayPass)
		{
			m_RenderPasses.emplace_back(pOverlayPass);
			pOverlayPass->OnStackAttach();
		}

		inline void PopPass(RenderPass* pPass)
		{
			auto Iter = std::find(m_RenderPasses.begin(), m_RenderPasses.end(), pPass);

			if (Iter != m_RenderPasses.end()) 
			{
				pPass->OnStackDetach();
				m_RenderPasses.erase(Iter);
				m_PassInsertIndex--;
			}
		}

		inline void PopPassOverlay(RenderPass* pOverlayPass)
		{
			auto Iter = std::find(m_RenderPasses.begin(), m_RenderPasses.end(), pOverlayPass);

			if (Iter != m_RenderPasses.end())
			{
				pOverlayPass->OnStackDetach();
				m_RenderPasses.erase(Iter);
			}
		}

		template <class RenderPassType>
		inline void PushPassBehind(RenderPass* pPass)
		{
			for (uint32_t i = 0; i < m_RenderPasses.size(); ++i)
			{
				if (dynamic_cast<RenderPassType*>(m_RenderPasses[i]))
				{
					//TODO insert Tech at i++
				}
			}
		}

		inline void ReloadBuffers()
		{
			for (RenderPass* Pass : m_RenderPasses)
			{
				Pass->CreateResources();
			}
		}

		inline void ReloadPipelines()
		{
			for (RenderPass* Pass : m_RenderPasses)
			{
				Pass->LoadPipeline();
			}
		}

		std::vector<RenderPass*>::iterator begin() { return m_RenderPasses.begin(); }
		std::vector<RenderPass*>::iterator end() { return m_RenderPasses.end(); }

	private:
		std::vector<RenderPass*> m_RenderPasses;
		unsigned int m_PassInsertIndex = 0;
	};
}