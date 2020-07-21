#pragma once

#include <Insight/Core.h>

#include "Insight/Rendering/Geometry/Model.h"

namespace Insight {

	using namespace Microsoft::WRL;

	class GeometryManager
	{
	public:
		typedef std::vector<StrongModelPtr> SceneModels;
	public:
		GeometryManager();
		~GeometryManager();

		bool Init();

		SceneModels* GetSceneModels() { return &m_Models; }

		// Issue draw commands to all models attached to the geometry manager.
		void Render(RenderPass RenderPass);
		// Gather all geometry in the scene and uplaod their constant buffers to the GPU.
		// Should only be called once, before 'Render()'. Does not draw models.
		void GatherGeometry();
		// Reset incrementor for model geometry gather phase.
		// See 'GatherGeometry()' for more information.
		void PostRender();
		// UnRegister all model in the model cache. Usually used 
		// when switching scenes.
		void FlushModelCache();
		
		// Register a model to be drawn in the geometry pass
		inline void RegisterModel(StrongModelPtr Model) { m_Models.push_back(Model); }
		// Unregister a model to not be drawn in the geometry pass
		void UnRegisterModel(StrongModelPtr Model);

	private:
		SceneModels m_Models;  
		D3D12_GPU_VIRTUAL_ADDRESS m_CbvUploadHeapHandle;
		D3D12_GPU_VIRTUAL_ADDRESS m_CbvMaterialHeapHandle;
		
		UINT8* m_CbvPerObjectGPUAddress = nullptr;
		UINT8* m_CbvMaterialGPUAddress = nullptr;

		ID3D12Resource* m_ConstantBufferUploadHeaps = nullptr;
		ID3D12Resource* m_ConstantBufferMaterialUploadHeaps = nullptr;
		ID3D12GraphicsCommandList* m_pScenePassCommandList = nullptr;
		ID3D12GraphicsCommandList* m_pShadowPassCommandList = nullptr;

		int ConstantBufferPerObjectAlignedSize = (sizeof(CB_VS_PerObject) + 255) & ~255;
		int ConstantBufferPerObjectMaterialAlignedSize = (sizeof(CB_PS_VS_PerObjectAdditives) + 255) & ~255;
		UINT32 m_PerObjectCBDrawOffset = 0u;
		UINT32 m_GPUAddressUploadOffset = 0u;
	};

}
