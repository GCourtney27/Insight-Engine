#pragma once

#include <Runtime/CoreMacros.h>

#include "Runtime/Rendering/Texture.h"


namespace Insight {

	

	class INSIGHT_API ieD3D11Texture : public Texture
	{
	public:
		ieD3D11Texture(IE_TEXTURE_INFO createInfo);
		virtual ~ieD3D11Texture();
		

		// Destroy and release texture resources.
		virtual void Destroy() override;
		// Binds the texture to the pipeline to be used in the scene pass.
		virtual void BindForDeferredPass() override;
		// Binds the texture to the pipeline to be used in the forward render pass.
		virtual void BindForForwardPass() override;

	private:
		// Load  the texture from disk and initialize.
		bool Init();
		// Load a DDS texture from disk.
		void InitDDSTexture();
		// Load  generic texture file from disk.
		void InitTextureFromFile();
		uint32_t GetShaderRegisterLocation();
	private:
		Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pDeviceContext;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureView;
		uint32_t m_ShaderRegister;
	};

}
