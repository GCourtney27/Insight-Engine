#pragma once

#include <Insight/Core.h>

#include "Insight/Rendering/Geometry/Model.h"

namespace Insight {

	using namespace Microsoft::WRL;

	class ModelManager
	{
	public:
		typedef std::vector<StrongModelPtr> SceneModels;
	public:
		ModelManager();
		~ModelManager();

		static ModelManager& Get() { return *s_Instance; }

		bool Init();
		void Render();

		SceneModels* GetSceneModels() { return &m_Models; }

		void UploadVertexDataToGPU();
		void PostRender();
		void FlushModelCache();
		
		void PushModel(StrongModelPtr model) { m_Models.push_back(model); }

	private:
		SceneModels m_Models;  
		D3D12_GPU_VIRTUAL_ADDRESS m_CbvUploadHeapHandle;
		D3D12_GPU_VIRTUAL_ADDRESS m_CbvMaterialHeapHandle;
		
		UINT8* m_CbvPerObjectGPUAddress = nullptr;
		UINT8* m_CbvMaterialGPUAddress = nullptr;

		ID3D12Resource* m_ConstantBufferUploadHeaps = nullptr;
		ID3D12Resource* m_ConstantBufferMaterialUploadHeaps = nullptr;
		ID3D12GraphicsCommandList* m_pCommandList = nullptr;

		int ConstantBufferPerObjectAlignedSize = (sizeof(CB_VS_PerObject) + 255) & ~255;
		int ConstantBufferPerObjectMaterialAlignedSize = (sizeof(CB_PS_VS_PerObjectAdditives) + 255) & ~255;
		UINT32 m_PerObjectCBDrawOffset = 0u;
		UINT32 m_GPUAddressUploadOffset = 0u;
	private:
		static ModelManager* s_Instance;
	};

}
