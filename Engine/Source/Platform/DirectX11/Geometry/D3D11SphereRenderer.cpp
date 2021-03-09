#include <Engine_pch.h>

#include "D3D11SphereRenderer.h"

#include "Platform/DirectX11/Direct3D11Context.h"

namespace Insight {



	void ieD3D11SphereRenderer::Init(float radius, int slices, int segments, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	{
		m_pDevice = pDevice;
		m_pDeviceContext = pDeviceContext;
		m_Radius = radius;
		m_Slices = slices;
		m_Segments = segments;
		ResourceSetup();
	}

	void ieD3D11SphereRenderer::ResourceSetup()
	{
		using namespace DirectX;
		HRESULT hr;

		std::vector< SimpleVertex > verts;
		verts.resize((m_Segments + 1) * m_Slices + 2);

		const float _pi = XM_PI;
		const float _2pi = XM_2PI;

		verts[0].position = XMFLOAT4(0, m_Radius, 0, 1);
		for (int lat = 0; lat < m_Slices; lat++)
		{
			float a1 = _pi * (float)(lat + 1) / (m_Slices + 1);
			float sin1 = sinf(a1);
			float cos1 = cosf(a1);

			for (int lon = 0; lon <= m_Segments; lon++)
			{
				float a2 = _2pi * (float)(lon == m_Segments ? 0 : lon) / m_Segments;
				float sin2 = sinf(a2);
				float cos2 = cosf(a2);

				verts[lon + lat * (m_Segments + 1) + 1].position = XMFLOAT4(sin1 * cos2 * m_Radius, cos1 * m_Radius, sin1 * sin2 * m_Radius, 1);
			}
		}
		verts[verts.size() - 1].position = XMFLOAT4(0, -m_Radius, 0, 1);

		int nbFaces = (int)verts.size();
		int nbTriangles = nbFaces * 2;
		int nbIndexes = nbTriangles * 3;
		std::vector< int >  triangles(nbIndexes);


		int i = 0;
		for (int lon = 0; lon < m_Segments; lon++)
		{
			triangles[i++] = lon + 2;
			triangles[i++] = lon + 1;
			triangles[i++] = 0;
		}

		//Middle
		for (int lat = 0; lat < m_Slices - 1; lat++)
		{
			for (int lon = 0; lon < m_Segments; lon++)
			{
				int current = lon + lat * (m_Segments + 1) + 1;
				int next = current + m_Segments + 1;

				triangles[i++] = current;
				triangles[i++] = current + 1;
				triangles[i++] = next + 1;

				triangles[i++] = current;
				triangles[i++] = next + 1;
				triangles[i++] = next;
			}
		}

		//Bottom Cap
		for (int lon = 0; lon < m_Segments; lon++)
		{
			triangles[i++] = (int)verts.size() - 1;
			triangles[i++] = (int)verts.size() - (lon + 2) - 1;
			triangles[i++] = (int)verts.size() - (lon + 1) - 1;
		}
		m_TriangleSize = (int)verts.size();
		m_IndexSize = (int)triangles.size();
		
		D3D11_BUFFER_DESC VertexBufferDesc = {};
		VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		VertexBufferDesc.ByteWidth = sizeof(ScreenSpaceVertex) * m_TriangleSize;
		VertexBufferDesc.CPUAccessFlags = 0;
		VertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA VertexBufferData = {};
		VertexBufferData.pSysMem = verts.data();

		hr = m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexBufferData, m_pVertexBuffer.GetAddressOf());
		//ThrowIfFailed(hr);

		D3D11_BUFFER_DESC IndexBufferDesc = {};
		IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		IndexBufferDesc.ByteWidth = sizeof(DWORD) * m_IndexSize;
		IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		IndexBufferDesc.CPUAccessFlags = 0U;
		IndexBufferDesc.MiscFlags = 0U;

		D3D11_SUBRESOURCE_DATA IndexBufferData = {};
		IndexBufferData.pSysMem = triangles.data();
		hr = m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexBufferData, m_pIndexBuffer.GetAddressOf());
		//ThrowIfFailed(hr);
	}

	void ieD3D11SphereRenderer::Render()
	{
		UINT Offsets = 0U;
		UINT Stride = sizeof(SimpleVertex);
		m_pDeviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &Stride, &Offsets);
		m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		m_pDeviceContext->DrawIndexed(m_IndexSize, 0, 0);
	}

}
