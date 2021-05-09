/*
	Single include for client side use of the renderer.
*/
#pragma once

#include "Runtime/Graphics/Public/IRenderContext.h"
#include "Runtime/Graphics/Public/ICommandContext.h"
#include "Runtime/Graphics/Public/CommonStructs.h"
#include "Runtime/Graphics/Public/CommonEnums.h"
#include "Runtime/Graphics/Public/CommonStructHelpers.h"
#include "Runtime/Graphics/Public/IGeometryBufferManager.h"
#include "Runtime/Graphics/Public/ResourceManagement/ITextureManager.h"
#include "Runtime/Graphics/Public/IDescriptorHeap.h"
#include "Runtime/Graphics/Public/Resource/IVertexBuffer.h"
#include "Runtime/Graphics/Public/Resource/IIndexBuffer.h"
#include "Runtime/Graphics/Public/Resource/IPixelBuffer.h"
#include "Runtime/Graphics/Public/Resource/IDepthBuffer.h"

//
// Factories
//
//#if IE_RENDERER_WITH_D3D12
#include "Platform/DirectX12/Public/D3D12RenderContextFactory.h"
//#endif // IE_RENDERER_WITH_D3D12
