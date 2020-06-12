#pragma once

#include <Insight/Core.h>

#include "Platform/DirectX12/Descriptor_Heap_Wrapper.h"


namespace Insight {

	using Microsoft::WRL::ComPtr;

	class Texture
	{
	public:
		enum eTextureType
		{
			// Per Object
			ALBEDO = 0,
			NORMAL = 1,
			ROUGHNESS = 2,
			METALLIC = 3,
			AO = 4,
			// Sky Sphere
			SKY_IRRADIENCE = 5,
			SKY_ENVIRONMENT_MAP = 6,
			SKY_BRDF_LUT = 7,
			SKY_DIFFUSE = 8,
		};

		struct IE_Texture_Create_Info
		{
			UINT width;
			UINT height;
			UINT mipLevels;
			eTextureType type;
			std::wstring filepath;
		};

		typedef UINT TextureHandle;
	public:
		Texture(const std::wstring& filepath, eTextureType& textureType, CD3DX12_CPU_DESCRIPTOR_HANDLE& srvHeapHandle);
		Texture();
		//Texture(Texture&& texture) noexcept;
		~Texture();
	
		bool Init(const std::wstring& filepath, eTextureType testureType, CDescriptorHeapWrapper& srvHeapHandle);
		bool InitTextureFromFile(const std::wstring& filepath, eTextureType& testureType, CDescriptorHeapWrapper& srvHeapHandle);
		void Bind();

		inline const UINT32 GetSrvHeapHandle() { return m_GPUHeapIndex; }
		inline const std::string& GetName() const { return m_Name; }
		inline const std::wstring& GetFilepath() const { return m_Filepath; }
		inline const UINT64& GetWidth() const { return m_TextureDesc.Width; }
		inline const UINT64& GetHeight() const { return m_TextureDesc.Height; }
		inline const UINT16& GetMipLevels() const { return m_TextureDesc.MipLevels; }
		inline const DXGI_FORMAT& GetFormat() const { return m_TextureDesc.Format; }
		void Destroy();

	private:
		void InitDDSTexture(const std::wstring& filepath, CDescriptorHeapWrapper& srvHeapHandle);

		DXGI_FORMAT GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID);
		WICPixelFormatGUID GetConvertToWICFormat(WICPixelFormatGUID& wicFormatGUID);
		int GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat);
		int LoadImageDataFromFile(BYTE** imageData, LPCWSTR filename, int& bytesPerRow);
		
	private:
		ID3D12GraphicsCommandList*	m_pCommandList = nullptr;

		ID3D12Resource*				m_pTexture;
		ID3D12Resource*				m_pTextureUploadHeap;
		D3D12_RESOURCE_DESC			m_TextureDesc = {};
		eTextureType				m_TextureType;
		TextureHandle				m_GPUHeapIndex = 0u;

		static UINT32 s_NumSceneTextures;

//#if defined IE_DEBUG
		std::string m_Name = "";
		std::wstring m_Filepath = L"";
		inline const std::wstring& GetFilepath() { return m_Filepath; }
//#endif

	};

}