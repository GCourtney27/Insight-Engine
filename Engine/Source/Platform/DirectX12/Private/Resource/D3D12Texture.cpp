#include <Engine_pch.h>

#include "Platform/DirectX12/Public/Resource/D3D12Texture.h"
#include "Platform/DirectX12/Private/D3D12BackendCore.h"

#include "Runtime/Graphics/Public/IDevice.h"
#include "Runtime/Graphics/Public/ICommandContext.h"
#include "Platform/DirectX12/Private/LinearAllocator.h"
#include "Platform/DirectX12/Public/D3D12CommandContext.h"
#include "Platform/Public/Utility/COMException.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			void D3D12Texture::Create2D(UInt64 RowPitchBytes, UInt64 Width, UInt64 Height, EFormat Format, const void* InitData)
			{
				Destroy();

				ID3D12Device* pD3D12Device = RCast<ID3D12Device*>(g_pDevice->GetNativeDevice());

				m_UsageState = RS_CopyDestination;

				m_Width = (UInt32)Width;
				m_Height = (UInt32)Height;
				m_Depth = 1;

				D3D12_RESOURCE_DESC texDesc = {};
				texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				texDesc.Width = Width;
				texDesc.Height = (UINT)Height;
				texDesc.DepthOrArraySize = 1;
				texDesc.MipLevels = 1;
				texDesc.Format = (DXGI_FORMAT)Format;
				texDesc.SampleDesc.Count = 1;
				texDesc.SampleDesc.Quality = 0;
				texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
				texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

				D3D12_HEAP_PROPERTIES HeapProps;
				HeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
				HeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
				HeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
				HeapProps.CreationNodeMask = 1;
				HeapProps.VisibleNodeMask = 1;

				pD3D12Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &texDesc,
					(D3D12_RESOURCE_STATES)m_UsageState, nullptr, IID_PPV_ARGS(&m_pID3D12Resource));

				m_pID3D12Resource->SetName(L"Texture");

				D3D12_SUBRESOURCE_DATA texResource;
				texResource.pData = InitData;
				texResource.RowPitch = RowPitchBytes;
				texResource.SlicePitch = RowPitchBytes * Height;

				UInt32 NumSubresources = 1;
				UINT64 uploadBufferSize = GetRequiredIntermediateSize(GetResource(), 0, NumSubresources);

				ICommandContext& InitContext = ICommandContext::Begin(L"Texture Init");
				D3D12CommandContext& D3D12InitContext = *DCast<D3D12CommandContext*>(&InitContext);
				{

					// copy data to the intermediate upload heap and then schedule a copy from the upload heap to the default texture
					DynAlloc mem = D3D12InitContext.ReserveUploadMemory(uploadBufferSize);
					UpdateSubresources(RCast<ID3D12GraphicsCommandList*>(InitContext.GetNativeContext()), GetResource(), mem.Buffer.GetResource(), 0, 0, NumSubresources, &texResource);
					InitContext.TransitionResource(*this, RS_GenericRead);
				}
				// Execute the command list and wait for it to finish so we can release the upload buffer
				InitContext.Finish(true);


				if (m_hCpuDescriptorHandle.ptr == IE_INVALID_GPU_ADDRESS)
					m_hCpuDescriptorHandle = AllocateDescriptor(pD3D12Device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				pD3D12Device->CreateShaderResourceView(m_pID3D12Resource.Get(), nullptr, m_hCpuDescriptorHandle);
			}

			void D3D12Texture::CreateCube(UInt64 RowPitchBytes, UInt64 Width, UInt64 Height, EFormat Format, const void* InitialData)
			{
				Destroy();

				ID3D12Device* pD3D12Device = RCast<ID3D12Device*>(g_pDevice->GetNativeDevice());

				m_UsageState = RS_CopyDestination;

				m_Width = (uint32_t)Width;
				m_Height = (uint32_t)Height;
				m_Depth = 6;

				D3D12_RESOURCE_DESC texDesc = {};
				texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				texDesc.Width = Width;
				texDesc.Height = (UINT)Height;
				texDesc.DepthOrArraySize = 6;
				texDesc.MipLevels = 1;
				texDesc.Format = (DXGI_FORMAT)Format;
				texDesc.SampleDesc.Count = 1;
				texDesc.SampleDesc.Quality = 0;
				texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
				texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

				D3D12_HEAP_PROPERTIES HeapProps;
				HeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
				HeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
				HeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
				HeapProps.CreationNodeMask = 1;
				HeapProps.VisibleNodeMask = 1;


				HRESULT hr = pD3D12Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &texDesc,
					(D3D12_RESOURCE_STATES)m_UsageState, nullptr, IID_PPV_ARGS(&m_pID3D12Resource));
				ThrowIfFailed(hr, TEXT(""));

				m_pID3D12Resource->SetName(L"Texture");

				D3D12_SUBRESOURCE_DATA texResource;
				texResource.pData = InitialData;
				texResource.RowPitch = RowPitchBytes;
				texResource.SlicePitch = texResource.RowPitch * Height;

				UInt32 NumSubresources = 1;
				UINT64 uploadBufferSize = GetRequiredIntermediateSize(GetResource(), 0, NumSubresources);

				ICommandContext& InitContext = ICommandContext::Begin(L"Texture Init");
				D3D12CommandContext& D3D12InitContext = *DCast<D3D12CommandContext*>(&InitContext);
				{

					// copy data to the intermediate upload heap and then schedule a copy from the upload heap to the default texture
					DynAlloc mem = D3D12InitContext.ReserveUploadMemory(uploadBufferSize);
					UpdateSubresources(RCast<ID3D12GraphicsCommandList*>(InitContext.GetNativeContext()), GetResource(), mem.Buffer.GetResource(), 0, 0, NumSubresources, &texResource);
					InitContext.TransitionResource(*this, RS_GenericRead);
				}
				// Execute the command list and wait for it to finish so we can release the upload buffer
				InitContext.Finish(true);

				if (m_hCpuDescriptorHandle.ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
					m_hCpuDescriptorHandle = AllocateDescriptor(pD3D12Device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
				srvDesc.Format = (DXGI_FORMAT)Format;
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
				srvDesc.TextureCube.MipLevels = 1;
				srvDesc.TextureCube.MostDetailedMip = 0;
				srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
				pD3D12Device->CreateShaderResourceView(m_pID3D12Resource.Get(), &srvDesc, m_hCpuDescriptorHandle);
			}

			void D3D12Texture::Destroy()
			{
				D3D12GPUResource::Destroy();
				m_hCpuDescriptorHandle.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
			}
		}
	}
}