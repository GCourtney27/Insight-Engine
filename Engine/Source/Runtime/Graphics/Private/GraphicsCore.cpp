#include <Engine_pch.h>

#include <Runtime/Graphics/Public/GraphicsCore.h>

#include "Runtime/Graphics/Public/Resource/ITexture.h"
#include "Runtime/Graphics/Public/ResourceManagement/ModelManager.h"
#include "Runtime/Graphics/Public/ResourceManagement/MaterialManager.h"


namespace Insight
{
	namespace Graphics
	{
		// -----------------------------
		//	Extern Variable Definitions
		// -----------------------------
		//
		ICommandManager* g_pCommandManager = NULL;
		IContextManager* g_pContextManager = NULL;
		IDevice* g_pDevice = NULL;
		IGeometryBufferManager* g_pGeometryManager = NULL;
		IConstantBufferManager* g_pConstantBufferManager = NULL;
		ITextureManager* g_pTextureManager = NULL;
		ITexture* g_DefaultTextures[DT_NumDefaultTextures];
		IDescriptorHeap* g_pTextureHeap = NULL;
		StaticGeometryManager g_StaticGeometryManager;
		MaterialManager g_MaterialManager;

		// -----------------------------
		//	Function Definitions
		// -----------------------------
		//
		ITexture* GetDefaultTexture(EDefaultTexture TexID)
		{
			IE_ASSERT(TexID < DT_NumDefaultTextures && TexID > -1);
			return g_DefaultTextures[TexID];
		}
	}
}
