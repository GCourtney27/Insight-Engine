#pragma once

#include <Runtime/Core.h>

namespace Insight
{
	namespace Graphics
	{
		struct ViewPort
		{
			float TopLeftX;
			float TopLeftY;
			float Width;
			float Height;
			float MinDepth;
			float MaxDepth;
		};

		struct Rect
		{
			Int32 Left;
			Int32 Top;
			Int32 Right;
			Int32 Bottom;
		};

		struct SampleDesc
		{
			UInt32 Count;
			UInt32 Quality;
		};

		struct MipRegion
		{
			UInt32 Width;
			UInt32 Height;
			UInt32 Depth;
		};

		struct ResourceDesc
		{
			EResourceDimension Dimension;
			UInt64 Alignment;
			UInt64 Width;
			UInt32 Height;
			UInt16 DepthOrArraySize;
			UInt16 MipLevels;
			EFormat Format;
			SampleDesc SampleDesc;
			ETextureLayout Layout;
			EResourceFlags Flags;
			MipRegion SamplerFeedbackMipRegion;
		};

		struct IESwapChainDescription
		{
			UInt32 Width;
			UInt32 Height;
			UInt32 BufferCount;
			SampleDesc SampleDesc;
			EFormat Format;
			void* NativeWindow;
		};

		struct DepthStencilValue
		{
			float Depth;
			UInt8 Stencil;
		};
		
		struct ClearValue
		{
			EFormat Format;
			float Color[4];
			DepthStencilValue DepthStencil;
		};

		struct ShaderByteCode
		{
			const void* pShaderByteCode;
			UInt64 ByteCodeLength;
		};

		struct RenderTargetBlendDesc
		{
			bool BlendEnable;
			bool LogicOpEnable;
			EBlend SourceBlend;
			EBlend DestBlend;
			EBlendOp BlendOp;
			EBlend SourceBlendAlpha;
			EBlend DestBlendAlpha;
			EBlendOp BlendOpAlpha;
			ELogicOp LocigOp;
			UInt8 RenderTargetWriteMask;
		};

		struct SubResourceData
		{
			const void* pData;
			Int64 RowPitch;
			Int64 SlicePitch;
		};

		struct BlendDesc
		{
			bool AlphaToCoverageEnable;
			bool IndependentBlendEnable;
			RenderTargetBlendDesc  RenderTarget[8];
		};

		struct StencilOpDesc
		{
			EStencilOp StencilFailOp;
			EStencilOp StencilDepthFailOp;
			EStencilOp StencilPassOp;
			EComparisonFunc StencilFunc;
		};

		struct DepthStencilStateDesc
		{
			bool DepthEnable;
			EDepthWriteMask DepthWriteMask;
			EComparisonFunc DepthFunc;
			bool StencilEnabled;
			UInt8 StencilReadMask;
			UInt8 StencilWriteMask;
			StencilOpDesc FrontFace;
			StencilOpDesc BackFace;

		};

		struct RasterizerDesc
		{
			EFillMode FillMode;
			ECullMode CullMode;
			bool FrontCounterClockwise;
			UInt32 DepthBias;
			float DepthBiasClamp;
			float SlopeScaledDepthBias;
			bool DepthClipEnabled;
			bool MultiSampleEnable;
			bool AntialiazedLineEnabled;
			UInt32 ForcedSampleCount;
			EConseritiveRasterMode ConservativeRaster;
		};

		struct InputElementDesc
		{
			char* SemanticName;
			UInt32 SemanticIndex;
			EFormat Format;
			UInt32 InputSlot;
			UInt32 AlignedByteOffset;
			EInputClassification InputSlotClass;
			UInt32 InstanceDataStepRate;
		};

		struct InputLayoutDesc
		{
			const InputElementDesc* pInputElementDescs;
			UInt32 NumElements;
		};

		struct PipelineStateDesc
		{
			class IRootSignature* pRootSignature;
			ShaderByteCode VertexShader;
			ShaderByteCode PixelShader;
			ShaderByteCode DomainShader;
			ShaderByteCode HullShader;
			ShaderByteCode GeometryShader;
			BlendDesc BlendState;
			UInt32 SampleMask;
			RasterizerDesc RasterizerDesc;
			DepthStencilStateDesc DepthStencilState;
			InputLayoutDesc InputLayout;
			EIndexBufferStripCutValue IBStripCutValue;
			EPrimitiveTopologyType PrimitiveTopologyType;
			UInt32 NumRenderTargets;
			EFormat RTVFormats[8];
			EFormat DSVFormat;
			SampleDesc SampleDesc;
			UInt32 NodeMask;
			void* CachedPSO;
			EPipelineStateFlags Flags;
		};

		struct DescriptorRange
		{
			EDescriptorRangeType Type;
			UInt32 NumDescriptors;
			UInt32 BaseShaderRegister;
			UInt32 RegisterSpace;
			UInt32 OffsetInDescriptorsFromTableStart;
		};

		struct RootDescriptorTable
		{
			UInt32 NumDescriptorRanges;
			const DescriptorRange* pDescriptorRanges;
		};

		struct RootConstants
		{
			UInt32 ShaderRegister;
			UInt32 RegisterSpace;
			UInt32 Num32BitValues;
		};

		struct RootDescriptor
		{
			UInt32 ShaderRegister;
			UInt32 RegisterSpace;
		};

		struct SamplerDesc
		{
			EFilter Filter;
			ETextureAddressMode AddressU;
			ETextureAddressMode AddressV;
			ETextureAddressMode AddressW;
			float MipLODBias;
			UInt32 MaxAnisotropy;
			EComparisonFunc ComparisonFunc;
			float BorderColor[4];
			float MinLOD;
			float MaxLOD;
		};

		struct StaticSamplerDesc
		{
			EFilter Filter;
			ETextureAddressMode AddressU;
			ETextureAddressMode AddressV;
			ETextureAddressMode AddressW;
			float MipLODBias; 
			UInt32 MaxAnisotropy;
			EComparisonFunc ComparisonFunc;
			EStaticBorderColor BorderColor;
			float MinLOD;
			float MaxLOD;
			UInt32 ShaderRegister;
			UInt32 RegisterSpace;
			EShaderVisibility ShaderVisibility;
		};

		struct RootParameter
		{
			ERootParameterType ParameterType;
			union
			{
				RootDescriptorTable DescriptorTable;
				RootConstants Constants;
				RootDescriptor Descriptor;
			};
			EShaderVisibility ShaderVisibility;
		};

		struct RootSignatureDesc
		{
			UInt32 NumParams;
			const RootParameter* pParameters;
			UInt32 NumStaticSamplers;
			const StaticSamplerDesc* pStaticSamplers;
			ERootSignatureFlags Flags;
		};

		struct CpuDescriptorHandle
		{
			UInt64 Ptr;
		};
		
		struct GpuDescriptorHandle
		{
			UInt64 Ptr;
		};

		struct Blob
		{

		};
	}
}
