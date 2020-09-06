#include <Engine_pch.h>

#include "Texture.h"

#include "Insight/Core/Application.h"
#include "Insight/Utilities/String_Helper.h"
#include "Renderer.h"

#include "Platform/Windows/DirectX_12/Direct3D12_Context.h"

#define CBVSRV_HEAP_TEXTURE_START_SLOT 7

namespace Insight {


	Texture::Texture(IE_TEXTURE_INFO CreateInfo)
		: m_TextureInfo(CreateInfo)
	{
	}

	Texture::~Texture()
	{
	}
}