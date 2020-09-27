#pragma once

#include <Retina/Core.h>

namespace Retina {

	using Microsoft::WRL::ComPtr;

	class ieD3D11SphereRenderer
	{
	public:
		void Init(float radius, int slices, int segments, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		void ResourceSetup();
		void Render();

	private:
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		ComPtr<ID3D11Buffer> m_pVertexBuffer;
		ComPtr<ID3D11Buffer> m_pIndexBuffer;
		int m_Slices;
		int m_Segments;
		int m_TriangleSize;
		int m_IndexSize;
		float m_Radius;
		struct SimpleVertex
		{
			DirectX::XMFLOAT4 position;
		};
	};

}
