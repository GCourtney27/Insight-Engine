#pragma once


#include "Insight/Core.h"

namespace Insight {



	class Texture
	{
	public:
		static enum eTextureType
		{
			// These values aligned with D3D12 shader registers,
			// use causion when editing their numerical values
			ALBEDO = 0,
			NORMAL = 1,
			ROUGHNESS = 2,
			METALLIC = 3,
			SPECULAR = 4,
			AO = 5,
		};
	public:
		Texture(const std::wstring& filepath, eTextureType& textureType, CD3DX12_CPU_DESCRIPTOR_HANDLE& srvHeapHandle);
		Texture();
		~Texture();
	
		bool Init(const std::wstring& filepath, eTextureType& testureType, CD3DX12_CPU_DESCRIPTOR_HANDLE& srvHeapHandle);
		void Bind();

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
		ID3D12GraphicsCommandList* m_pCommandList = nullptr;
		eTextureType m_TextureType;

#if defined IE_DEBUG
		std::string m_Name = "";
		std::wstring m_Filepath = L"";
#endif
	};

}