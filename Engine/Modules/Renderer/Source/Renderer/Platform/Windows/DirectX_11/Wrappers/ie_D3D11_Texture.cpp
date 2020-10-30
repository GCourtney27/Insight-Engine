#include <Renderer_pch.h>

#include "ie_D3D11_Texture.h"

#include "Platform/Windows/DirectX_11/Direct3D11_Context.h"

#include <DirectX11/TK/Inc/DDSTextureLoader.h>
#include <DirectX11/TK/Inc/WICTextureLoader.h>

namespace Insight {



	ieD3D11Texture::ieD3D11Texture(IE_TEXTURE_INFO CreateInfo)
		: Texture(CreateInfo)
	{
		Init();
	}

	ieD3D11Texture::~ieD3D11Texture()
	{
	}

	void ieD3D11Texture::Destroy()
	{
	}

	void ieD3D11Texture::BindForDeferredPass()
	{
		m_pDeviceContext->PSSetShaderResources(m_ShaderRegister, 1, m_pTextureView.GetAddressOf());
	}

	void ieD3D11Texture::BindForForwardPass()
	{
		if (m_TextureInfo.IsCubeMap) {
			constexpr uint32_t RenderPassShaderRegisterOffset = 4U;
			m_pDeviceContext->PSSetShaderResources(m_ShaderRegister - RenderPassShaderRegisterOffset, 1, m_pTextureView.GetAddressOf());
		}
		else {
			constexpr uint32_t RenderPassShaderRegisterOffset = 5U;
			m_pDeviceContext->PSSetShaderResources(m_ShaderRegister - RenderPassShaderRegisterOffset, 1, m_pTextureView.GetAddressOf());
		}
	}

	bool ieD3D11Texture::Init()
	{
		Direct3D11Context& RenderContext = Renderer::GetAs<Direct3D11Context>();

		m_pDevice = &RenderContext.GetDevice();
		m_pDeviceContext = &RenderContext.GetDeviceContext();

		std::string Filepath = StringHelper::WideToString(m_TextureInfo.Filepath);

		std::string FileExtension = StringHelper::GetFileExtension(Filepath);
		if (FileExtension == "dds") {
			InitDDSTexture();
		}
		else {
			InitTextureFromFile();
		}

		m_ShaderRegister = GetShaderRegisterLocation();

		return true;
	}

	void ieD3D11Texture::InitDDSTexture()
	{
		HRESULT hr = DirectX::CreateDDSTextureFromFile(m_pDevice.Get(), m_pDeviceContext.Get(), m_TextureInfo.Filepath.c_str(), nullptr, m_pTextureView.GetAddressOf());
		ThrowIfFailed(hr, "Failed to load D3D 11 DDS texture from file.");
	}

	void ieD3D11Texture::InitTextureFromFile()
	{
		HRESULT hr = DirectX::CreateWICTextureFromFile(m_pDevice.Get(), m_TextureInfo.Filepath.c_str(), nullptr, m_pTextureView.GetAddressOf());
		ThrowIfFailed(hr, "Failed to load D3D 11 WIC texture from file.");
	}

	uint32_t ieD3D11Texture::GetShaderRegisterLocation()
	{
		switch (m_TextureInfo.Type)
		{
		case eTextureType::eTextureType_Albedo:
		{
			return 5;
			break;
		}
		case eTextureType::eTextureType_Normal:
		{
			return 6;
			break;
		}
		case eTextureType::eTextureType_Roughness:
		{
			return 7;
			break;
		}
		case eTextureType::eTextureType_Metallic:
		{
			return 8;
			break;
		}
		case eTextureType::eTextureType_Opacity:
		{
			return 8;
			break;
		}
		case eTextureType::eTextureType_AmbientOcclusion:
		{
			return 9;
			break;
		}
		case eTextureType::eTextureType_Translucency:
		{
			return 9;
			break;
		}
		case eTextureType::eTextureType_SkyIrradience:
		{
			return 11;
			break;
		}
		case eTextureType::eTextureType_SkyRadianceMap:
		{
			return 12;
			break;
		}
		case eTextureType::eTextureType_IBLBRDFLUT:
		{
			return 13;
			break;
		}
		case eTextureType::eTextureType_SkyDiffuse:
		{
			return 14;
			break;
		}
		default:
		{
			IE_CORE_ERROR("Failed to determine shader register of texture with type \"{0}\" in D3D 11 context.", m_TextureInfo.Type);
		}
		}
		return - 1;
	}

}
