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
			// These values aligned with D3D12 shader registers,
			// use caution when editing their numerical values
			ALBEDO = 0,
			NORMAL = 1,
			ROUGHNESS = 2,
			METALLIC = 3,
			SPECULAR = 4,
			AO = 5,
			// SKysphere
			SKY_DIFFUSE = 6,
			SKY_IRRADIENCE = 7,
			SKY_ENVIRONMENT_MAP = 8

		};

#define ALBEDO_DESCRIPTOR_OFFSET eTextureType::ALBEDO
#define NORMAL_DESCRIPTOR_OFFSET eTextureType::NORMAL
#define ROUGHNESS_DESCRIPTOR_OFFSET eTextureType::ROUGHNESS
#define METALLIC_DESCRIPTOR_OFFSET eTextureType::METALLIC
#define SPECULAR_DESCRIPTOR_OFFSET eTextureType::SPECULAR
#define AO_DESCRIPTOR_OFFSET eTextureType::AO

	public:
		Texture(const std::wstring& filepath, eTextureType& textureType, CD3DX12_CPU_DESCRIPTOR_HANDLE& srvHeapHandle);
		Texture();
		~Texture();
	
		bool Init(const std::wstring& filepath, eTextureType& testureType, CDescriptorHeapWrapper& srvHeapHandle);
		bool Init(const std::wstring& filepath, eTextureType& testureType);
		void Bind();

		inline const UINT64& GetWidth() const { return m_TextureDesc.Width; }
		inline const UINT64& GetHeight() const { return m_TextureDesc.Height; }
		inline const UINT16& GetMipLevels() const { return m_TextureDesc.MipLevels; }
		inline const DXGI_FORMAT& GetFormat() const { return m_TextureDesc.Format; }
		void Destroy();

	private:
		void InitFromDDSTexture(const std::wstring& filepath);

		DXGI_FORMAT GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID);
		WICPixelFormatGUID GetConvertToWICFormat(WICPixelFormatGUID& wicFormatGUID);
		int GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat);
		int LoadImageDataFromFile(BYTE** imageData, LPCWSTR filename, int& bytesPerRow);
		
	private:
		ID3D12GraphicsCommandList*	m_pCommandList = nullptr;
		
		ComPtr<ID3D12Resource>		m_pTexture;
		ComPtr<ID3D12Resource>		m_pTextureUploadHeap;
		D3D12_RESOURCE_DESC			m_TextureDesc = {};
		eTextureType				m_TextureType;
		UINT32						m_GPUHeapIndex = 0u;

		static UINT32 s_NumSceneTextures;

//#if defined IE_DEBUG
		std::string m_Name = "";
		std::wstring m_Filepath = L"";
		inline const std::wstring& GetFilepath() { return m_Filepath; }
//#endif

	};

}