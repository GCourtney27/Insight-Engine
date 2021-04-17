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

namespace Insight
{
	namespace Graphics
	{
		// Forward Declares
		//
		// Classes
		class IDevice;
		class IPipelineState;
		class IRootSignature;
		class IVertexBuffer;
		class IIndexBuffer;
		
		// Structs
		struct PipelineStateDesc;
		struct RootSignatureDesc;


		// Extern Variables
		// 
		// Command context managment overlord.
		extern class ICommandManager* g_pCommandManager;
		// Graphics context managmetn overlord.
		extern class IContextManager* g_pContextManager;
		// Graphics rendering device.
		extern class IDevice* g_pDevice;
		// Geometry buffer overloard.
		extern class IGeometryManager* g_pGeometryManager;

		// UIDs
		//
		typedef UInt32 VertexBufferUID;
		typedef UInt32 IndexBufferUID;


		// Utility Methods
		//
		template <typename DerivedType, typename BaseType, typename ... InitArgs>
		inline DerivedType* CreateRenderComponentObject(BaseType** ppBase, InitArgs ... args)
		{
			*ppBase = new DerivedType(args...);
			DerivedType* pDericedClass = DCast<DerivedType*>(*ppBase);
			IE_ASSERT(pDericedClass != NULL);

			return pDericedClass;
		}


#define IE_INVALID_VERTEX_BUFFER_HANDLE (VertexBufferUID)(-1)
#define IE_INVALID_INDEX_BUFFER_HANDLE (IndexBufferUID)(-1)
	}
}
