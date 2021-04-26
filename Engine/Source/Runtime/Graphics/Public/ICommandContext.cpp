#include <Engine_pch.h>

#include "Runtime/Graphics/Public/GraphicsCore.h"

#include "Runtime/Graphics/Public/ICommandContext.h"


namespace Insight
{
	namespace Graphics
	{

		IContextManager::IContextManager()
		{
		}

		IContextManager::~IContextManager()
		{
			for (size_t i = 0; i < cx_ContextPoolSize; ++i)
			{
				for (size_t j = 0; j < m_ContextPool[i].size(); ++j)
				{
					delete m_ContextPool[i][j];
				}
			}
		}

		ICommandContext& ICommandContext::Begin(const EString& ID)
		{
			ICommandContext* NewContext = g_pContextManager->AllocateContext(ECommandListType::CLT_Direct);
			NewContext->SetID(ID);
			return *NewContext;
		}
	}
}