/*
	Single include for client side use of the renderer.
*/
#pragma once

#include "Graphics/Public/IDevice.h"
#include "Graphics/Public/ISwapChain.h"
#include "Graphics/Public/CommonEnums.h"
#include "Graphics/Public/CommonStructs.h"
#include "Graphics/Public/IRenderContext.h"
#include "Graphics/Public/ICommandContext.h"
#include "Graphics/Public/IDescriptorHeap.h"
#include "Graphics/Public/CommonStructHelpers.h"
#include "Graphics/Public/ICommandManager.h"
#include "Graphics/Public/IGeometryBufferManager.h"
#include "Graphics/Public/Resource/IVertexBuffer.h"
#include "Graphics/Public/Resource/IIndexBuffer.h"
#include "Graphics/Public/Resource/IPixelBuffer.h"
#include "Graphics/Public/Resource/IDepthBuffer.h"
#include "Graphics/Public/ResourceManagement/ITextureManager.h"
#include "Graphics/Public/ResourceManagement/IConstantBufferManager.h"


//
// Factories
//
//#if IE_RENDERER_WITH_D3D12
#include "Platform/DirectX12/Public/D3D12RenderContextFactory.h"
//#endif // IE_RENDERER_WITH_D3D12
