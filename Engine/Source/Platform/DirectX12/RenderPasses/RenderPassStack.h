/*
	File: Render_Pass_Stack.h
	Source: None

	Author: Garrett Courtney

	Description:
	A container for all render passes that make up a image renered to the main byffer.
	All passes are organized in layers to make adding and removeing techniques quick and easy.
*/

#pragma once

#include <Runtime/CoreMacros.h>

#include <vector>

#include "RenderPasses.h"

namespace Insight {


	class INSIGHT_API RenderPassStack
	{
	public:
		RenderPassStack() = default;
		~RenderPassStack() = default;

		/*
			Push a pass to the stack to be rendered.
		*/
		inline void PushPass(RenderPass* pPass)
		{
			m_RenderPasses.emplace(m_RenderPasses.begin() + m_PassInsertIndex, pPass);
			pPass->OnStackAttach();
			m_PassInsertIndex++;
		}

		/*
			Push a pass to be rendered last in the render stack.
		*/
		inline void PushPassOverlay(RenderPass* pOverlayPass)
		{
			m_RenderPasses.emplace_back(pOverlayPass);
			pOverlayPass->OnStackAttach();
		}

		/*
			Remove a pass from the render stack.
		*/
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

		/*
			Remove a overlay from the renderstack.
		*/
		inline void PopPassOverlay(RenderPass* pOverlayPass)
		{
			auto Iter = std::find(m_RenderPasses.begin(), m_RenderPasses.end(), pOverlayPass);

			if (Iter != m_RenderPasses.end())
			{
				pOverlayPass->OnStackDetach();
				m_RenderPasses.erase(Iter);
			}
		}

		/*
			Add a render pass succeeding a spacified pass.
		*/
		template <class RenderPassType>
		inline void PushPassBehind(RenderPass* pPass)
		{
			auto Iter = m_RenderPasses.begin();
			for (Iter; Iter != m_RenderPasses.end(); ++Iter)
			{
				if (dynamic_cast<RenderPassType*>(*Iter))
				{
					m_RenderPasses.insert(Iter++, pPass);
					break;
				}
			}
		}

		/*
			Add a render pass proceeding a spacified pass.
		*/
		template <class RenderPassType>
		inline void PushPassInFrontOf(RenderPass* pPass)
		{
			auto Iter = m_RenderPasses.begin();
			for (Iter; Iter != m_RenderPasses.end(); ++Iter)
			{
				if (dynamic_cast<RenderPassType*>(*Iter))
				{
					m_RenderPasses.insert(Iter--, pPass);
					break;
				}
			}
		}

		/*
			Add a render pass between two specified passes.
		*/
		template <class RenderPassFirst, class RenderPassLast>
		inline void PushPassBetween(RenderPass* pPass)
		{
			auto Front = m_RenderPasses.begin();
			for (Front; Front != m_RenderPasses.end(); ++Front)
			{
				if (dynamic_cast<RenderPassFirst*>(*Front))
				{
					auto Last = Front;
					for (Last; Last != m_RenderPasses.end(); ++Last)
					{
						if (dynamic_cast<RenderPassLast*>(*Last))
						{
							m_RenderPasses.insert(Last--, pPass);
						}
					}
				}
			}
		}

		/*
			Reload the buffers each pass uses. Usually called when the window is resizing and buffers need to reflect it.
		*/
		inline void ReloadBuffers()
		{
			for (RenderPass* Pass : m_RenderPasses)
			{
				Pass->CreateResources();
			}
		}

		/*
			Reload the pipelines in use by the render passes. Usually called when shaders need to be reloaded.
		*/
		inline void ReloadPipelines()
		{
			for (RenderPass* Pass : m_RenderPasses)
			{
				Pass->LoadPipeline();
			}
		}

		/*
			Returns the first pass present in the render pass stack.
		*/
		std::vector<RenderPass*>::iterator begin() { return m_RenderPasses.begin(); }
		
		/*
			Returns the last pass present in the render pass stack.
		*/
		std::vector<RenderPass*>::iterator end() { return m_RenderPasses.end(); }

	private:
		/*
			Contains all passes that make up a image rendered on the screen.
		*/
		std::vector<RenderPass*> m_RenderPasses;

		/*
			Insertion index for the next added pass to use.
		*/
		unsigned int m_PassInsertIndex = 0;
	};
}