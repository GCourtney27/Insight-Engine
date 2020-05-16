#pragma once

#include <Insight/Core.h>

#include "Insight/Rendering/Geometry/Static_Mesh.h"

namespace Insight {

	using namespace Microsoft::WRL;
	using TransformInstanceQueue = std::queue<Transform>;


	class ModelManager
	{
	public:
		ModelManager();
		~ModelManager();

		static ModelManager& Get() { return *s_Instance; }

		bool Init();
		void Draw();

		//void RequestDrawCall(Transform& instanceTransform);

		void UploadVertexDataToGPU();

		bool LoadMeshFromFile(const std::string& filePath, bool async = true);
		void FlushModelCache();
		
	private:

	private:
		std::vector< std::shared_ptr<StaticMesh> > m_Models;
		//std::unordered_map< std::shared_ptr<Model>, TransformInstanceQueue > m_Models;

		ID3D12Resource* m_ConstantBufferUploadHeaps = nullptr;
		ID3D12GraphicsCommandList* m_pCommandList = nullptr;
		int ConstantBufferPerObjectAlignedSize = (sizeof(CB_VS_PerObject) + 255) & ~255;

	private:
		static ModelManager* s_Instance;
	};

}
