#include <ie_pch.h>

#include "Texture.h"

#include "Insight/Core/Application.h"
#include "Insight/Utilities/String_Helper.h"
#include "Platform/DirectX12/Direct3D12_Context.h"

namespace Insight {

	UINT32 Texture::s_NumSceneTextures = 0u;

	Texture::Texture(const std::wstring& filepath, eTextureType& textureType, CD3DX12_CPU_DESCRIPTOR_HANDLE& srvHeapHandle)
	{
		//Init(filepath, textureType, srvHeapHandle);
	}

	Texture::Texture()
	{
		
	}

	Texture::~Texture()
	{
		Destroy();
	}

	void Texture::Destroy()
	{
		/*m_pTextureBuffer->Release();
		m_pTextureBufferUploadHeap->Release();
		m_pCommandList = nullptr;*/
	}

	bool Texture::Init(const std::wstring& filepath, eTextureType& textureType, CDescriptorHeapWrapper& srvHeapHandle)
	{
		Direct3D12Context& graphicsContext = Direct3D12Context::Get();
		m_pCommandList = &graphicsContext.GetCommandList();
		m_TextureType = textureType;

		std::string strFilePath = StringHelper::WideToString(filepath);
		std::string extension = StringHelper::GetFileExtension(strFilePath);

		if (extension == "dds") {
			InitDDSTexture(filepath, srvHeapHandle);
		}
		else {
			InitTextureFromFile(filepath, textureType, srvHeapHandle);
		}

		return true;
	}

	bool Texture::InitTextureFromFile(const std::wstring& filepath, eTextureType& textureType, CDescriptorHeapWrapper& srvHeapHandle)
	{
		HRESULT hr;
		
		Direct3D12Context& graphicsContext = Direct3D12Context::Get();
		ID3D12Device* pDevice = &graphicsContext.GetDeviceContext();
		

		BYTE* imageData = 0;
		int imageBytesPerRow = 0;
		int imageSize = LoadImageDataFromFile(&imageData, filepath.c_str(), imageBytesPerRow);
		if (imageSize <= 0) {
			IE_CORE_ERROR("Failed to create image from file: {0}", StringHelper::WideToString(filepath));
			return false;
		}
		hr = pDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&m_TextureDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&m_pTexture)
		);
		if (FAILED(hr)) {
			IE_CORE_ERROR("Failed to create resource heap for texture asset");
			return false;
		}
//#if defined IE_DEBUG
		m_Name = StringHelper::GetFilenameFromDirectoryW(filepath);
		std::wstring debugName = L"Texture Buffer Resource Heap" + StringHelper::StringToWide(m_Name);
		m_pTexture->SetName(debugName.c_str());
//#endif

		UINT64 textureUploadBufferSize;
		pDevice->GetCopyableFootprints(&m_TextureDesc, 0, 1, 0, nullptr, nullptr, nullptr, &textureUploadBufferSize);
		//ComPtr<ID3D12Resource> textureBufferUploadHeap;

		hr = graphicsContext.GetDeviceContext().CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(textureUploadBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_pTextureUploadHeap)
		);
		if (FAILED(hr)) {
			IE_CORE_ERROR("Failed to create commited resource for texture buffer");
			return false;
		}

		m_pTextureUploadHeap->SetName(L"Texture Buffer Upload Resource Heap");

		D3D12_SUBRESOURCE_DATA textureData = {};
		textureData.pData = &imageData[0];
		textureData.RowPitch = imageBytesPerRow;
		textureData.SlicePitch = static_cast<UINT>(imageBytesPerRow) * m_TextureDesc.Height;

		UpdateSubresources(m_pCommandList, m_pTexture.Get(), m_pTextureUploadHeap.Get(), 0, 0, 1, &textureData);

		m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pTexture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Format = m_TextureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		pDevice->CreateShaderResourceView(m_pTexture.Get(), &srvDesc, srvHeapHandle.hCPU(6 + s_NumSceneTextures));
		
		m_GPUHeapIndex = s_NumSceneTextures;
		s_NumSceneTextures++;

		delete imageData;
		return true;
	}

	void Texture::InitDDSTexture(const std::wstring& filepath, CDescriptorHeapWrapper& srvHeapHandle)
	{
		HRESULT hr;
		Direct3D12Context& graphicsContext = Direct3D12Context::Get();
		ID3D12Device* pDevice = &graphicsContext.GetDeviceContext();

		std::unique_ptr<uint8_t[]> ddsData;
		std::vector<D3D12_SUBRESOURCE_DATA> subresources;
		hr = LoadDDSTextureFromFile(pDevice, filepath.c_str(), m_pTexture.ReleaseAndGetAddressOf(), ddsData, subresources);
		ThrowIfFailed(hr, "Failed to load DDS texture from file");
		const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_pTexture.Get(), 0, static_cast<UINT>(subresources.size()));

		CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
		auto desc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

		hr = pDevice->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_pTextureUploadHeap)
		);
		ThrowIfFailed(hr, "Failed to create committed resource for texture buffer upload heap");
		UpdateSubresources(m_pCommandList, m_pTexture.Get(), m_pTextureUploadHeap.Get(), 0, 0, static_cast<UINT>(subresources.size()), subresources.data());

		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pTexture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		m_pCommandList->ResourceBarrier(1, &barrier);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Format = m_TextureDesc.Format;
		
		srvDesc.ViewDimension = (m_TextureType >= eTextureType::SKY_IRRADIENCE) ? D3D12_SRV_DIMENSION_TEXTURECUBE : D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		pDevice->CreateShaderResourceView(m_pTexture.Get(), &srvDesc, srvHeapHandle.hCPU(6 + s_NumSceneTextures));
		m_GPUHeapIndex = s_NumSceneTextures;
		s_NumSceneTextures++;

		//hr = m_pCommandList->Close();
		//ThrowIfFailed(hr, "Failed to close command list while loading dds texture.");
		//ID3D12CommandList* ppCommandLists[] = { m_pCommandList };
		//graphicsContext.GetCommandQueue().ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	}

	void Texture::Bind()
	{
		Direct3D12Context& graphicsContext = Direct3D12Context::Get();
		CDescriptorHeapWrapper& cbvSrvHeapStart = graphicsContext.GetCBVSRVDescriptorHeap();
		const unsigned int numRTVs = graphicsContext.GetNumRTVs();

		switch (m_TextureType) {
		case eTextureType::ALBEDO:
		{
			m_pCommandList->SetGraphicsRootDescriptorTable(5, cbvSrvHeapStart.hGPU(6 + m_GPUHeapIndex));
			break;
		}
		case eTextureType::NORMAL:
		{
			m_pCommandList->SetGraphicsRootDescriptorTable(6, cbvSrvHeapStart.hGPU(6 + m_GPUHeapIndex));
			break;
		}
		case eTextureType::ROUGHNESS:
		{
			m_pCommandList->SetGraphicsRootDescriptorTable(7, cbvSrvHeapStart.hGPU(6 + m_GPUHeapIndex));
			break;
		}
		case eTextureType::METALLIC:
		{
			m_pCommandList->SetGraphicsRootDescriptorTable(8, cbvSrvHeapStart.hGPU(6 + m_GPUHeapIndex));
			break;
		}
		case eTextureType::AO:
		{
			m_pCommandList->SetGraphicsRootDescriptorTable(9, cbvSrvHeapStart.hGPU(6 + m_GPUHeapIndex));
			break;
		}
		case eTextureType::SKY_IRRADIENCE:
		{
			m_pCommandList->SetGraphicsRootDescriptorTable(10, cbvSrvHeapStart.hGPU(6 + m_GPUHeapIndex));
			break;
		}
		case eTextureType::SKY_ENVIRONMENT_MAP:
		{
			m_pCommandList->SetGraphicsRootDescriptorTable(11, cbvSrvHeapStart.hGPU(6 + m_GPUHeapIndex));
			break;
		}
		case eTextureType::SKY_BRDF_LUT:
		{
			m_pCommandList->SetGraphicsRootDescriptorTable(12, cbvSrvHeapStart.hGPU(6 + m_GPUHeapIndex));
			break;
		}
		case eTextureType::SKY_DIFFUSE:
		{
			m_pCommandList->SetGraphicsRootDescriptorTable(13, cbvSrvHeapStart.hGPU(6 + m_GPUHeapIndex));
			break;
		}
		default:
		{
			IE_CORE_WARN("Failed to bind texture {0}", m_Name);
			break;
		}
		}
	}

	int Texture::LoadImageDataFromFile(BYTE** imageData, LPCWSTR filename, int& bytesPerRow)
	{
		HRESULT hr;

		// we only need one instance of the imaging factory to create decoders and frames
		static IWICImagingFactory* wicFactory;

		// reset decoder, frame and converter since these will be different for each image we load
		IWICBitmapDecoder* wicDecoder = NULL;
		IWICBitmapFrameDecode* wicFrame = NULL;
		IWICFormatConverter* wicConverter = NULL;

		bool imageConverted = false;

		if (wicFactory == NULL)
		{
			// Initialize the COM library
			CoInitialize(NULL);

			// create the WIC factory
			hr = CoCreateInstance(
				CLSID_WICImagingFactory,
				NULL,
				CLSCTX_INPROC_SERVER,
				IID_PPV_ARGS(&wicFactory)
			);
			if (FAILED(hr)) return 0;
		}

		// load a decoder for the image
		hr = wicFactory->CreateDecoderFromFilename(
			filename,                        // Image we want to load in
			NULL,                            // This is a vendor ID, we do not prefer a specific one so set to null
			GENERIC_READ,                    // We want to read from this file
			WICDecodeMetadataCacheOnLoad,    // We will cache the metadata right away, rather than when needed, which might be unknown
			&wicDecoder                      // the wic decoder to be created
		);
		if (FAILED(hr)) return 0;

		// get image from decoder (this will decode the "frame")
		hr = wicDecoder->GetFrame(0, &wicFrame);
		if (FAILED(hr)) return 0;

		// get wic pixel format of image
		WICPixelFormatGUID pixelFormat;
		hr = wicFrame->GetPixelFormat(&pixelFormat);
		if (FAILED(hr)) return 0;

		// get size of image
		UINT textureWidth, textureHeight;
		hr = wicFrame->GetSize(&textureWidth, &textureHeight);
		if (FAILED(hr)) return 0;

		// we are not handling sRGB types in this tutorial, so if you need that support, you'll have to figure
		// out how to implement the support yourself

		// convert wic pixel format to dxgi pixel format
		DXGI_FORMAT dxgiFormat = GetDXGIFormatFromWICFormat(pixelFormat);

		// if the format of the image is not a supported dxgi format, try to convert it
		if (dxgiFormat == DXGI_FORMAT_UNKNOWN)
		{
			// get a dxgi compatible wic format from the current image format
			WICPixelFormatGUID convertToPixelFormat = GetConvertToWICFormat(pixelFormat);

			// return if no dxgi compatible format was found
			if (convertToPixelFormat == GUID_WICPixelFormatDontCare) return 0;

			// set the dxgi format
			dxgiFormat = GetDXGIFormatFromWICFormat(convertToPixelFormat);

			// create the format converter
			hr = wicFactory->CreateFormatConverter(&wicConverter);
			if (FAILED(hr)) return 0;

			// make sure we can convert to the dxgi compatible format
			BOOL canConvert = FALSE;
			hr = wicConverter->CanConvert(pixelFormat, convertToPixelFormat, &canConvert);
			if (FAILED(hr) || !canConvert) return 0;

			// do the conversion (wicConverter will contain the converted image)
			hr = wicConverter->Initialize(wicFrame, convertToPixelFormat, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom);
			if (FAILED(hr)) return 0;

			// this is so we know to get the image data from the wicConverter (otherwise we will get from wicFrame)
			imageConverted = true;
		}

		int bitsPerPixel = GetDXGIFormatBitsPerPixel(dxgiFormat); // number of bits per pixel
		bytesPerRow = (textureWidth * bitsPerPixel) / 8; // number of bytes in each row of the image data
		int imageSize = bytesPerRow * textureHeight; // total image size in bytes

		// allocate enough memory for the raw image data, and set imageData to point to that memory
		*imageData = (BYTE*)malloc(imageSize);

		// copy (decoded) raw image data into the newly allocated memory (imageData)
		if (imageConverted)
		{
			// if image format needed to be converted, the wic converter will contain the converted image
			hr = wicConverter->CopyPixels(0, bytesPerRow, imageSize, *imageData);
			if (FAILED(hr)) return 0;
		}
		else
		{
			// no need to convert, just copy data from the wic frame
			hr = wicFrame->CopyPixels(0, bytesPerRow, imageSize, *imageData);
			if (FAILED(hr)) return 0;
		}

		// now describe the texture with the information we have obtained from the image
		m_TextureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		m_TextureDesc.Alignment = 0; // may be 0, 4KB, 64KB, or 4MB. 0 will let runtime decide between 64KB and 4MB (4MB for multi-sampled textures)
		m_TextureDesc.Width = textureWidth; // width of the texture
		m_TextureDesc.Height = textureHeight; // height of the texture
		m_TextureDesc.DepthOrArraySize = 1; // if 3d image, depth of 3d image. Otherwise an array of 1D or 2D textures (we only have one image, so we set 1)
		m_TextureDesc.MipLevels = 1; // Number of mipmaps. We are not generating mipmaps for this texture, so we have only one level
		m_TextureDesc.Format = dxgiFormat; // This is the dxgi format of the image (format of the pixels)
		m_TextureDesc.SampleDesc.Count = 1; // This is the number of samples per pixel, we just want 1 sample
		m_TextureDesc.SampleDesc.Quality = 0; // The quality level of the samples. Higher is better quality, but worse performance
		m_TextureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; // The arrangement of the pixels. Setting to unknown lets the driver choose the most efficient one
		m_TextureDesc.Flags = D3D12_RESOURCE_FLAG_NONE; // no flags

		// return the size of the image. remember to delete the image once your done with it (in this tutorial once its uploaded to the gpu)
		return imageSize;
	}

	DXGI_FORMAT Texture::GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID)
	{
		if (wicFormatGUID == GUID_WICPixelFormat128bppRGBAFloat) return DXGI_FORMAT_R32G32B32A32_FLOAT;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBAHalf) return DXGI_FORMAT_R16G16B16A16_FLOAT;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBA) return DXGI_FORMAT_R16G16B16A16_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA) return DXGI_FORMAT_R8G8B8A8_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppBGRA) return DXGI_FORMAT_B8G8R8A8_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppBGR) return DXGI_FORMAT_B8G8R8X8_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA1010102XR) return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;

		else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA1010102) return DXGI_FORMAT_R10G10B10A2_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat16bppBGRA5551) return DXGI_FORMAT_B5G5R5A1_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat16bppBGR565) return DXGI_FORMAT_B5G6R5_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppGrayFloat) return DXGI_FORMAT_R32_FLOAT;
		else if (wicFormatGUID == GUID_WICPixelFormat16bppGrayHalf) return DXGI_FORMAT_R16_FLOAT;
		else if (wicFormatGUID == GUID_WICPixelFormat16bppGray) return DXGI_FORMAT_R16_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat8bppGray) return DXGI_FORMAT_R8_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat8bppAlpha) return DXGI_FORMAT_A8_UNORM;

		else return DXGI_FORMAT_UNKNOWN;
	}

	WICPixelFormatGUID Texture::GetConvertToWICFormat(WICPixelFormatGUID& wicFormatGUID)
	{
		if (wicFormatGUID == GUID_WICPixelFormatBlackWhite) return GUID_WICPixelFormat8bppGray;
		else if (wicFormatGUID == GUID_WICPixelFormat1bppIndexed) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat2bppIndexed) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat4bppIndexed) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat8bppIndexed) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat2bppGray) return GUID_WICPixelFormat8bppGray;
		else if (wicFormatGUID == GUID_WICPixelFormat4bppGray) return GUID_WICPixelFormat8bppGray;
		else if (wicFormatGUID == GUID_WICPixelFormat16bppGrayFixedPoint) return GUID_WICPixelFormat16bppGrayHalf;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppGrayFixedPoint) return GUID_WICPixelFormat32bppGrayFloat;
		else if (wicFormatGUID == GUID_WICPixelFormat16bppBGR555) return GUID_WICPixelFormat16bppBGRA5551;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppBGR101010) return GUID_WICPixelFormat32bppRGBA1010102;
		else if (wicFormatGUID == GUID_WICPixelFormat24bppBGR) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat24bppRGB) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppPBGRA) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppPRGBA) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat48bppRGB) return GUID_WICPixelFormat64bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat48bppBGR) return GUID_WICPixelFormat64bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppBGRA) return GUID_WICPixelFormat64bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppPRGBA) return GUID_WICPixelFormat64bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppPBGRA) return GUID_WICPixelFormat64bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat48bppRGBFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wicFormatGUID == GUID_WICPixelFormat48bppBGRFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBAFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppBGRAFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBHalf) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wicFormatGUID == GUID_WICPixelFormat48bppRGBHalf) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wicFormatGUID == GUID_WICPixelFormat128bppPRGBAFloat) return GUID_WICPixelFormat128bppRGBAFloat;
		else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBFloat) return GUID_WICPixelFormat128bppRGBAFloat;
		else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBAFixedPoint) return GUID_WICPixelFormat128bppRGBAFloat;
		else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBFixedPoint) return GUID_WICPixelFormat128bppRGBAFloat;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBE) return GUID_WICPixelFormat128bppRGBAFloat;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppCMYK) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppCMYK) return GUID_WICPixelFormat64bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat40bppCMYKAlpha) return GUID_WICPixelFormat64bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat80bppCMYKAlpha) return GUID_WICPixelFormat64bppRGBA;

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
		else if (wicFormatGUID == GUID_WICPixelFormat32bppRGB) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppRGB) return GUID_WICPixelFormat64bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppPRGBAHalf) return GUID_WICPixelFormat64bppRGBAHalf;
#endif

		else return GUID_WICPixelFormatDontCare;
	}

	int Texture::GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat)
	{
		if (dxgiFormat == DXGI_FORMAT_R32G32B32A32_FLOAT) return 128;
		else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_FLOAT) return 64;
		else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_UNORM) return 64;
		else if (dxgiFormat == DXGI_FORMAT_R8G8B8A8_UNORM) return 32;
		else if (dxgiFormat == DXGI_FORMAT_B8G8R8A8_UNORM) return 32;
		else if (dxgiFormat == DXGI_FORMAT_B8G8R8X8_UNORM) return 32;
		else if (dxgiFormat == DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM) return 32;

		else if (dxgiFormat == DXGI_FORMAT_R10G10B10A2_UNORM) return 32;
		else if (dxgiFormat == DXGI_FORMAT_B5G5R5A1_UNORM) return 16;
		else if (dxgiFormat == DXGI_FORMAT_B5G6R5_UNORM) return 16;
		else if (dxgiFormat == DXGI_FORMAT_R32_FLOAT) return 32;
		else if (dxgiFormat == DXGI_FORMAT_R16_FLOAT) return 16;
		else if (dxgiFormat == DXGI_FORMAT_R16_UNORM) return 16;
		else if (dxgiFormat == DXGI_FORMAT_R8_UNORM) return 8;
		else if (dxgiFormat == DXGI_FORMAT_A8_UNORM) return 8;
		return -1;
	}

}