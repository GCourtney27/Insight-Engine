#pragma once


#include "Insight/Core.h"

namespace Insight {

	class Texture
	{
	public:
		Texture(const std::wstring& filepath, int mipLevels);
		Texture() {}
		~Texture();
	
		bool Init(const std::wstring& filepath, int mipLevels);

		inline const UINT64& GetWidth() const { return m_TextureDesc.Width; }
		inline const UINT64& GetHeight() const { return m_TextureDesc.Height; }
		inline const UINT16& GetMipLevels() const { return m_TextureDesc.MipLevels; }
		inline const DXGI_FORMAT& GetFormat() const { return m_TextureDesc.Format; }
		void Destroy();

	private:

		DXGI_FORMAT GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID);
		WICPixelFormatGUID GetConvertToWICFormat(WICPixelFormatGUID& wicFormatGUID);
		int GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat);
		int LoadImageDataFromFile(BYTE** imageData, LPCWSTR filename, int& bytesPerRow);
	private:
		D3D12_RESOURCE_DESC m_TextureDesc = {};
		ID3D12Resource* m_pTextureBuffer = nullptr;
		ID3D12Resource* m_pTextureBufferUploadHeap = nullptr;
	};

}