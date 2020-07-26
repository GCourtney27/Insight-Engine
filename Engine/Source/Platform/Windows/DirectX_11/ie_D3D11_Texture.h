#pragma once

#include <Insight/Core.h>

#include "Insight/Rendering/Texture.h"


namespace Insight {

	class INSIGHT_API ieD3D11Texture : public Texture
	{
	public:
		ieD3D11Texture(IE_TEXTURE_INFO createInfo);
		virtual ~ieD3D11Texture();
		

		// Destroy and release texture resources.
		virtual void Destroy() override;
		// Binds the texture to the pipeline to be drawn in the scene pass.
		virtual void Bind() override;

	private:
		// Load  the texture from disk and initialize.
		bool Init();
		// Load a DDS texture from disk.
		void InitDDSTexture();
		// Load  generic texture file from disk.
		void InitTextureFromFile();
		uint32_t GetShaderRegisterLocation();
	private:
		ComPtr<ID3D11Device> m_pDevice;
		ComPtr<ID3D11DeviceContext> m_pDeviceContext;

		ComPtr<ID3D11ShaderResourceView> m_pTextureView;
		uint32_t m_ShaderRegister;
	};

}
