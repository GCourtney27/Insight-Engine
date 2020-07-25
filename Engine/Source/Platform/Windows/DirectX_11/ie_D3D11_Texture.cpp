#include <ie_pch.h>

#include "ie_D3D11_Texture.h"

#include "Platform/Windows/DirectX_11/Direct3D11_Context.h"

#include <DirectX11/TK/Inc/DDSTextureLoader.h>
#include <DirectX11/TK/Inc/WICTextureLoader.h>

namespace Insight {



	ieD3D11Texture::ieD3D11Texture(IE_TEXTURE_INFO CreateInfo)
	{
		m_TextureInfo = CreateInfo;
		Init();
	}

	ieD3D11Texture::~ieD3D11Texture()
	{
	}

	void ieD3D11Texture::Destroy()
	{
	}

	void ieD3D11Texture::Bind()
	{
		m_pDeviceContext->PSSetShaderResources(m_TextureInfo.Type, 1, m_pTextureView.GetAddressOf());
	}

	bool ieD3D11Texture::Init()
	{
		Direct3D11Context* Context = reinterpret_cast<Direct3D11Context*>(&Renderer::Get());
		m_pDevice = &Context->GetDevice();
		m_pDeviceContext = &Context->GetDeviceContext();

		std::string Filepath = StringHelper::WideToString(m_TextureInfo.Filepath);
		m_TextureInfo.DisplayName = StringHelper::GetFilenameFromDirectory(Filepath);

		std::string FileExtension = StringHelper::GetFileExtension(Filepath);
		if (FileExtension == "dds") {
			InitDDSTexture();
		}
		else {
			InitTextureFromFile();
		}

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

}
