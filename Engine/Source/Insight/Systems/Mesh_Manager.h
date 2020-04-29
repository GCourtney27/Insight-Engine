#pragma once

#include "Insight/Core.h"

#include "Insight/Rendering/Geometry/Model.h"

namespace Insight {

	using namespace Microsoft::WRL;

	class ModelManager
	{
	public:
		ModelManager();
		~ModelManager();

		bool Init();
		void Draw();

		void Update();

		bool LoadMeshFromFile(const std::string& filePath, bool async = true);
		
	private:
		
	private:
		std::vector< std::shared_ptr<Model> > m_Models;
		static const uint8_t m_FrameBufferCount = 3u;
		uint8_t m_FrameIndex = 0u;

		ID3D12Resource* m_ConstantBufferUploadHeaps = nullptr;
		int ConstantBufferPerObjectAlignedSize = (sizeof(CB_VS_PerObject) + 255) & ~255;

	};

}
