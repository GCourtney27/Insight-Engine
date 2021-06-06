#include <Engine_pch.h>

#include "Platform/DirectX12/Public/ResourceManagement/D3D12TextureManager.h"

#include "Platform/DirectX12/Private/D3D12BackendCore.h"
#include "Graphics/Public/GraphicsCore.h"
#include "Graphics/Public/IDevice.h"
#include "Platform/DirectX12/Private/Utility/DDSTextureLoader.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			void D3D12TextureManager::Initialize()
			{
                UInt32 MagentaPixel = 0xFFFF00FF;
                m_DefaultTextures[DT_Magenta2D].Create2D(4, 1, 1, F_R8G8B8A8_UNorm, &MagentaPixel);
                UInt32 BlackOpaqueTexel = 0xFF000000;
                m_DefaultTextures[DT_BlackOpaque2D].Create2D(4, 1, 1, F_R8G8B8A8_UNorm, &BlackOpaqueTexel);
                UInt32 BlackTransparentTexel = 0x00000000;
                m_DefaultTextures[DT_BlackTransparent2D].Create2D(4, 1, 1, F_R8G8B8A8_UNorm, &BlackTransparentTexel);
                UInt32 WhiteOpaqueTexel = 0xFFFFFFFF;
                m_DefaultTextures[DT_WhiteOpaque2D].Create2D(4, 1, 1, F_R8G8B8A8_UNorm, &WhiteOpaqueTexel);
                UInt32 WhiteTransparentTexel = 0x00FFFFFF;
                m_DefaultTextures[DT_WhiteTransparent2D].Create2D(4, 1, 1, F_R8G8B8A8_UNorm, &WhiteTransparentTexel);
                UInt32 FlatNormalTexel = 0x00FF8080;
                m_DefaultTextures[DT_DefaultNormalMap].Create2D(4, 1, 1, F_R8G8B8A8_UNorm, &FlatNormalTexel);
                UInt32 BlackCubeTexels[6] = {};
                m_DefaultTextures[DT_BlackCubeMap].CreateCube(4, 1, 1, F_R8G8B8A8_UNorm, BlackCubeTexels);

                for (UInt32 i = 0; i < DT_NumDefaultTextures; ++i)
                {
                    g_DefaultTextures[i] = &m_DefaultTextures[i];
                }
			}
			
			void D3D12TextureManager::UnInitialize()
			{
				m_TextureCache.clear();
			}
			
            ITextureRef D3D12TextureManager::LoadTexture(const FString& FileName, EDefaultTexture Fallback, bool forceSRGB)
            {
                return FindOrLoadTexture(FileName, Fallback, forceSRGB);
            }

            IManagedTexture* D3D12TextureManager::FindOrLoadTexture(const FString& FileName, EDefaultTexture Fallback, bool forceSRGB)
			{
                D3D12ManagedTexture* pTexture = NULL;

                {
                    std::lock_guard<std::mutex> Guard(m_Mutex);

                    FString key = FileName;
                    if (forceSRGB)
                        key += L"_sRGB";

                    // Search for an existing managed texture
                    auto iter = m_TextureCache.find(key);
                    if (iter != m_TextureCache.end())
                    {
                        // If a texture was already created make sure it has finished loading before
                        // returning a point to it.
                        pTexture = iter->second.get();
                        pTexture->WaitForLoad();
                        return pTexture;
                    }
                    else
                    {
                        // If it's not found, create a new managed texture and start loading it.
                        pTexture = new D3D12ManagedTexture(key);
                        m_TextureCache[key].reset(pTexture);
                    }
                }

                // TODO Filesystem readfilesync
                DataBlob Data = FileSystem::ReadRawData(FileName.c_str());
                pTexture->CreateFromMemory(Data, Fallback, forceSRGB);

                // This was the first time it was requested, so indicate that the caller must read the file.
                return pTexture;
			}
            
            void D3D12TextureManager::DestroyTexture(const FString& Key)
            {
                std::lock_guard<std::mutex> Gaurd(m_Mutex);

                auto Iter = m_TextureCache.find(Key);
                if (Iter != m_TextureCache.end())
                    m_TextureCache.erase(Iter);
            }
            
            void D3D12ManagedTexture::CreateFromMemory(DataBlob memory, EDefaultTexture fallback, bool bForceSRGB)
            {
                ID3D12Device* pD3D12Device = RCast<ID3D12Device*>(g_pDevice->GetNativeDevice());
                D3D12Texture* pD3D12FallbackTexture = DCast<D3D12Texture*>(GetDefaultTexture(fallback));
                IE_ASSERT(pD3D12FallbackTexture != NULL)
                D3D12_CPU_DESCRIPTOR_HANDLE FallbackSRVHandle = pD3D12FallbackTexture->GetSRV();
                
                if (!memory.IsValid())
                {
                    m_hCpuDescriptorHandle = FallbackSRVHandle;
                }
                else
                {
                    // We probably have a texture to load, so let's allocate a new descriptor
                    m_hCpuDescriptorHandle = AllocateDescriptor(pD3D12Device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                    
                    HRESULT hr = CreateDDSTextureFromMemory(pD3D12Device, (const uint8_t*)memory.GetBufferPointer(), memory.GetDataSize(),
                        0, bForceSRGB, GetAddressOf(), m_hCpuDescriptorHandle);
                    if (SUCCEEDED(hr))
                    {
                        D3D12_RESOURCE_DESC Desc = GetResource()->GetDesc();
                        m_Width     = (uint32_t)Desc.Width;
                        m_Height    = Desc.Height;
                        m_Depth     = Desc.DepthOrArraySize;
                        
                        m_IsValid = true;
                    }
                    else
                    {
                        IE_LOG(Warning, TEXT("Failed to create dds texture from memory. Falling back to default texture: %i"), fallback)
                        pD3D12Device->CopyDescriptorsSimple(1, m_hCpuDescriptorHandle, FallbackSRVHandle,
                            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                    }
                }
                AssociateWithShaderVisibleHeap();
                m_IsLoading = false;
            }
        }
	}
}
