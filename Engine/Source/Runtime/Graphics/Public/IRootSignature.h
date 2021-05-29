#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/GraphicsCore.h"

namespace Insight
{
	namespace Graphics
	{
		class IRootParameter
		{
			friend class RootSignature;
		public:

			IRootParameter()
			{
				m_RootParam.ParameterType = (ERootParameterType)0xFFFFFFFF;
			}

			~IRootParameter()
			{
				Clear();
			}

			void Clear()
			{
				if (m_RootParam.ParameterType == RPT_DescriptorTable)
					delete[] m_RootParam.DescriptorTable.pDescriptorRanges;

				m_RootParam.ParameterType = (ERootParameterType)0xFFFFFFFF;
			}

			void InitAsConstants(UInt32 Register, UInt32 NumDwords, EShaderVisibility Visibility = SV_All, UInt32 Space = 0)
			{
				m_RootParam.ParameterType = RPT_32BitConstants;
				m_RootParam.ShaderVisibility = Visibility;
				m_RootParam.Constants.Num32BitValues = NumDwords;
				m_RootParam.Constants.ShaderRegister = Register;
				m_RootParam.Constants.RegisterSpace = Space;
			}

			void InitAsConstantBuffer(UInt32 Register, EShaderVisibility Visibility = SV_All, UInt32 Space = 0)
			{
				m_RootParam.ParameterType = RPT_ConstantBufferView;
				m_RootParam.ShaderVisibility = Visibility;
				m_RootParam.Descriptor.ShaderRegister = Register;
				m_RootParam.Descriptor.RegisterSpace = Space;
			}

			void InitAsBufferSRV(UInt32 Register, EShaderVisibility Visibility = SV_All, UInt32 Space = 0)
			{
				m_RootParam.ParameterType = RPT_ShaderResourceView;
				m_RootParam.ShaderVisibility = Visibility;
				m_RootParam.Descriptor.ShaderRegister = Register;
				m_RootParam.Descriptor.RegisterSpace = Space;
			}

			void InitAsBufferUAV(UInt32 Register, EShaderVisibility Visibility = SV_All, UInt32 Space = 0)
			{
				m_RootParam.ParameterType = RPT_UnorderedAccessView;
				m_RootParam.ShaderVisibility = Visibility;
				m_RootParam.Descriptor.ShaderRegister = Register;
				m_RootParam.Descriptor.RegisterSpace = Space;
			}

			void InitAsDescriptorRange(EDescriptorRangeType Type, UInt32 Register, UInt32 Count, EShaderVisibility Visibility = SV_All, UInt32 Space = 0)
			{
				InitAsDescriptorTable(1, Visibility);
				SetTableRange(0, Type, Register, Count, Space);
			}

			void InitAsDescriptorTable(UInt32 RangeCount, EShaderVisibility Visibility = SV_All)
			{
				m_RootParam.ParameterType = RPT_DescriptorTable;
				m_RootParam.ShaderVisibility = Visibility;
				m_RootParam.DescriptorTable.NumDescriptorRanges = RangeCount;
				m_RootParam.DescriptorTable.pDescriptorRanges = new DescriptorRange[RangeCount];
			}

			void SetTableRange(UInt32 RangeIndex, EDescriptorRangeType Type, UInt32 Register, UInt32 Count, UInt32 Space = 0)
			{
				DescriptorRange* range = const_cast<DescriptorRange*>(m_RootParam.DescriptorTable.pDescriptorRanges + RangeIndex);
				ZeroMem(range, sizeof(DescriptorRange));
				range->Type = Type;
				range->NumDescriptors = Count;
				range->BaseShaderRegister = Register;
				range->RegisterSpace = Space;
				range->OffsetInDescriptorsFromTableStart = IE_DESCRIPTOR_RANGE_OFFSET_APPEND;
			}

			const RootParameter& operator() (void) const { return m_RootParam; }


		protected:

			RootParameter m_RootParam;
		};

		namespace DX12
		{
			class D3D12DynamicDescriptorHeap;
		}

		// Maximum 64 DWORDS divied up amongst all root parameters.
		// Root constants = 1 DWORD * NumConstants
		// Root descriptor (CBV, SRV, or UAV) = 2 DWORDs each
		// Descriptor table pointer = 1 DWORD
		// Static samplers = 0 DWORDS (compiled into shader)
		class IRootSignature
		{
			friend DX12::D3D12DynamicDescriptorHeap;

		public:

			IRootSignature(UInt32 NumRootParams = 0, UInt32 NumStaticSamplers = 0)
				: m_Finalized(false)
				, m_NumParameters(NumRootParams)
			{
				Reset(NumRootParams, NumStaticSamplers);
			}

			~IRootSignature()
			{
			}

			virtual void DestroyAll(void) = 0;

			virtual void* GetNativeSignature() = 0;

			virtual void Initialize(const RootSignatureDesc& Desc) = 0;

			void Reset(UInt32 NumRootParams, UInt32 NumStaticSamplers = 0)
			{
				if (NumRootParams > 0)
					m_ParamArray.reset(new IRootParameter[NumRootParams]);
				else
					m_ParamArray = nullptr;
				m_NumParameters = NumRootParams;

				if (NumStaticSamplers > 0)
					m_SamplerArray.reset(new StaticSamplerDesc[NumStaticSamplers]);
				else
					m_SamplerArray = nullptr;
				m_NumSamplers = NumStaticSamplers;
				m_NumInitializedStaticSamplers = 0;
			}

			IRootParameter& operator[] (size_t EntryIndex)
			{
				IE_ASSERT(EntryIndex < m_NumParameters);
				return m_ParamArray.get()[EntryIndex];
			}

			const IRootParameter& operator[] (size_t EntryIndex) const
			{
				IE_ASSERT(EntryIndex < m_NumParameters);
				return m_ParamArray.get()[EntryIndex];
			}

			void InitStaticSampler(UInt32 Register, const SamplerDesc& NonStaticSamplerDesc,
				EShaderVisibility Visibility = SV_All);

			virtual void Finalize(const FString& name, ERootSignatureFlags Flags = RSF_None) = 0;

		protected:
			bool m_Finalized;
			UInt32 m_NumParameters;
			UInt32 m_NumSamplers;
			UInt32 m_NumInitializedStaticSamplers;
			UInt32 m_DescriptorTableBitMap;		// One bit is set for root parameters that are non-sampler descriptor tables.
			UInt32 m_SamplerTableBitMap;		// One bit is set for root parameters that are sampler descriptor tables.
			UInt32 m_DescriptorTableSize[16];	// Non-sampler descriptor tables need to know their descriptor count.
			std::unique_ptr<IRootParameter[]> m_ParamArray;
			std::unique_ptr<StaticSamplerDesc[]> m_SamplerArray;

		};
	}
}
