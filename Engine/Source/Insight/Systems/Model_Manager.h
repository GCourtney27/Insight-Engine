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
		void Draw();

		SceneModels* GetSceneModels() { return &m_Models; }

		void UploadVertexDataToGPU();
		void FlushModelCache();
		
		void PushModel(StrongModelPtr model) { m_Models.push_back(model); }

		//bool LoadMeshFromFile(const std::string& filePath, bool async = true);
	private:


	private:
		SceneModels m_Models;  

		ID3D12Resource* m_ConstantBufferUploadHeaps = nullptr;
		ID3D12GraphicsCommandList* m_pCommandList = nullptr;
		int ConstantBufferPerObjectAlignedSize = (sizeof(CB_VS_PerObject) + 255) & ~255;

	private:
		bool m_AutoInstanceEnabled = true;
	private:
		static ModelManager* s_Instance;
	};

}
