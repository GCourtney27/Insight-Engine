#pragma once

#include "EngineDefines.h"
#include <comdef.h>

/*
	Based on shuhuai's implementation in https://github.com/shuhuai/DeferredShadingD3D12
*/

namespace Insight {

	class INSIGHT_API CDescriptorHeapWrapper
	{
	public:
		CDescriptorHeapWrapper() { memset(this, 0, sizeof(*this)); }

		HRESULT Create(
			ID3D12Device* pDevice,
			D3D12_DESCRIPTOR_HEAP_TYPE Type,
			UINT NumDescriptors,
			bool bShaderVisible = false)
		{
			Desc.Type = Type;
			Desc.NumDescriptors = NumDescriptors;
			Desc.Flags = (bShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : (D3D12_DESCRIPTOR_HEAP_FLAGS)0);

			HRESULT hr = pDevice->CreateDescriptorHeap(&Desc,
				__uuidof(ID3D12DescriptorHeap),
				(void**)&pDH);
			if (FAILED(hr)) return hr;

			hCPUHeapStart = pDH->GetCPUDescriptorHandleForHeapStart();
			if (bShaderVisible)
			{
				hGPUHeapStart = pDH->GetGPUDescriptorHandleForHeapStart();
			}
			else
			{
				hGPUHeapStart.ptr = 0;
			}
			HandleIncrementSize = pDevice->GetDescriptorHandleIncrementSize(Desc.Type);
			return hr;
		}
		operator ID3D12DescriptorHeap* () { return pDH.Get(); }

		inline void Destroy()
		{
			pDH.Reset();
		}

		SIZE_T MakeOffsetted_SizeT(SIZE_T ptr, UINT index)
		{
			SIZE_T offsetted;
			offsetted = ptr + static_cast<SIZE_T>(index * HandleIncrementSize);
			return offsetted;
		}

		UINT64 MakeOffsetted_Uint64(UINT64 ptr, UINT index)
		{
			UINT64 offsetted;
			offsetted = ptr + static_cast<UINT64>(index * HandleIncrementSize);
			return offsetted;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE hCPU(UINT index)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle;
			handle.ptr = MakeOffsetted_SizeT(hCPUHeapStart.ptr, index);
			return handle;
		}
		D3D12_GPU_DESCRIPTOR_HANDLE hGPU(UINT index)
		{
			assert(Desc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
			D3D12_GPU_DESCRIPTOR_HANDLE handle;
			handle.ptr = MakeOffsetted_Uint64(hGPUHeapStart.ptr, index);
			return handle;
		}
		D3D12_DESCRIPTOR_HEAP_DESC Desc;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pDH;
		D3D12_CPU_DESCRIPTOR_HANDLE hCPUHeapStart;
		D3D12_GPU_DESCRIPTOR_HANDLE hGPUHeapStart;
		UINT HandleIncrementSize;
	};


}
