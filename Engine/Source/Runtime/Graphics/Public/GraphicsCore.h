#pragma once
#include "Runtime/Graphics/Public/CommonEnums.h"
#include "Runtime/Graphics/Public/CommonStructs.h"

#if RENDER_CORE_BUILD_DLL
#	define RENDER_API __declspec(dllexport)
#else
#	define RENDER_API __declspec(dllimport)
#endif // RENDER_CORE

#define IE_SIMULTANEOUS_RENDER_TARGET_COUNT 8
#define IE_DEFAULT_STENCIL_READ_MASK 0xFF
#define IE_DEFAULT_STENCIL_WRITE_MASK 0xFF
#define IE_DEFAULT_DEPTH_BIAS 0
#define IE_DEFAULT_DEPTH_BIAS_CLAMP 0.0f
#define IE_DEFAULT_SLOPE_SCALED_DEPTH_BIAS 0.0f
#define IE_INVALID_VERTEX_BUFFER_HANDLE (Insight::Graphics::VertexBufferUID)(-1)
#define IE_INVALID_INDEX_BUFFER_HANDLE (Insight::Graphics::IndexBufferUID)(-1)
#define	IE_APPEND_ALIGNED_ELEMENT	( 0xFFFFFFFF )
#define IE_MAX_CONSTANT_BUFFER_SIZE 256
#define IE_INVALID_CONSTANT_BUFFER_HANDLE (Insight::Graphics::ConstantBufferUID)(-1)

namespace Insight
{
	namespace Graphics
	{
		// ----------------------
		//	Forward Declarations
		// ----------------------
		//
		// Classes
		class IDevice;
		class ISwapChain;
		class IColorBuffer;
		class IDepthBuffer;
		class IGPUResource;
		class IPipelineState;
		class IRootSignature;
		class IVertexBuffer;
		class IIndexBuffer;
		class IDescriptorHeap;
		class ICommandManager;
		class IContextManager;
		class ICommandContext;
		class IConstantBufferManager;
		class IGeometryBufferManager;
		class IConstantBuffer;
		// Structs
		struct PipelineStateDesc;
		struct RootSignatureDesc;
		// Enums
		enum EResourceHeapType;

		// ------------------
		//	Extern Variables
		// ------------------
		// 
		// Command context managment overlord.
		extern ICommandManager* g_pCommandManager;
		// Graphics context management overlord.
		extern IContextManager* g_pContextManager;
		// Graphics rendering device.
		extern IDevice* g_pDevice;
		// Geometry buffer overloard.
		extern IGeometryBufferManager* g_pGeometryManager;
		// Constant buffer overlord.
		extern IConstantBufferManager* g_pConstantBufferManager;


		// ----------
		//	Typedefs
		// ----------
		//
		// UIDs
		typedef UInt32 VertexBufferUID;
		typedef UInt32 IndexBufferUID;
		typedef UInt32 ConstantBufferUID;

		// -----------------
		//	Utility Methods
		// -----------------
		//
		template <typename DerivedType, typename BaseType, typename ... InitArgs>
		inline DerivedType* CreateRenderComponentObject(BaseType** ppBase, InitArgs ... args)
		{
			*ppBase = new DerivedType(args...);
			DerivedType* pDericedClass = DCast<DerivedType*>(*ppBase);
			IE_ASSERT(pDericedClass != NULL);

			return pDericedClass;
		}
	}
}
