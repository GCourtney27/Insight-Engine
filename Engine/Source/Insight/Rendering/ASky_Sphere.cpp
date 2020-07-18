#include <ie_pch.h>

#include "ASky_Sphere.h"

#include "Insight/Runtime/Components/Actor_Component.h"
#include "Platform/Windows/DirectX12/Direct3D12_Context.h"
#include "Insight/Systems/File_System.h"

namespace Insight {



	ASkySphere::ASkySphere(ActorId id, ActorType type)
		: AActor(id, type)
	{
		m_Sphere.Init(10, 20, 20);
		Direct3D12Context& graphicsContext = Direct3D12Context::Get();
		m_pCommandList = &graphicsContext.GetScenePassCommandList();
		graphicsContext.AddSkySphere(this);
	}

	ASkySphere::~ASkySphere()
	{
	}

	bool ASkySphere::LoadFromJson(const rapidjson::Value& jsonSkySphere)
	{
		std::string diffuseMap;
		const rapidjson::Value& sky = jsonSkySphere["Sky"];
		json::get_string(sky[0], "Diffuse", diffuseMap);

		Direct3D12Context& graphicsContext = Direct3D12Context::Get();
		CDescriptorHeapWrapper& cbvSrvheap = graphicsContext.GetCBVSRVDescriptorHeap();

		Texture::IE_TEXTURE_INFO diffuseInfo;
		diffuseInfo.Filepath = StringHelper::StringToWide(FileSystem::GetRelativeAssetDirectory(diffuseMap));
		diffuseInfo.AssetDirectoryRelPath = diffuseMap;
		diffuseInfo.Type = Texture::eTextureType::SKY_DIFFUSE;
		diffuseInfo.GenerateMipMaps = true;
		diffuseInfo.IsCubeMap = true;
		m_Diffuse.Init(diffuseInfo, cbvSrvheap);

		return true;
	}

	bool ASkySphere::WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer)
	{
		Writer.StartObject(); // Start Write Actor
		{
			Writer.Key("Type");
			Writer.String("SkySphere");

			Writer.Key("DisplayName");
			Writer.String(SceneNode::GetDisplayName());

			Writer.Key("Transform");
			Writer.StartArray(); // Start Write Transform
			{
				Transform& Transform = SceneNode::GetTransformRef();
				ieVector3 Pos = Transform.GetPosition();
				ieVector3 Rot = Transform.GetRotation();
				ieVector3 Sca = Transform.GetScale();

				Writer.StartObject();
				// Position
				Writer.Key("posX");
				Writer.Double(Pos.x);
				Writer.Key("posY");
				Writer.Double(Pos.y);
				Writer.Key("posZ");
				Writer.Double(Pos.z);
				// Rotation
				Writer.Key("rotX");
				Writer.Double(Rot.x);
				Writer.Key("rotY");
				Writer.Double(Rot.y);
				Writer.Key("rotZ");
				Writer.Double(Rot.z);
				// Scale
				Writer.Key("scaX");
				Writer.Double(Sca.x);
				Writer.Key("scaY");
				Writer.Double(Sca.y);
				Writer.Key("scaZ");
				Writer.Double(Sca.z);

				Writer.EndObject();
			}
			Writer.EndArray(); // End Write Transform

			// Sky Attributes
			Writer.Key("Sky");
			Writer.StartArray();
			{
				Writer.StartObject();
				Writer.Key("Diffuse");
				Writer.String(m_Diffuse.GetAssetDirectoryRelPath().c_str());
				Writer.EndObject();
			}
			Writer.EndArray();

			Writer.Key("Subobjects");
			Writer.StartArray(); // Start Write SubObjects
			{
				for (size_t i = 0; i < m_NumComponents; ++i)
				{
					AActor::m_Components[i]->WriteToJson(Writer);
				}
			}
			Writer.EndArray(); // End Write SubObjects
		}
		Writer.EndObject(); // End Write Actor
		return true;
	}

	bool ASkySphere::OnInit()
	{
		return true;
	}

	bool ASkySphere::OnPostInit()
	{
		return true;
	}

	void ASkySphere::OnUpdate(const float& deltaMs)
	{
	}

	void ASkySphere::OnPreRender(XMMATRIX parentMat)
	{
	}

	void ASkySphere::RenderSky(ComPtr<ID3D12GraphicsCommandList> commandList)
	{
		m_Diffuse.Bind();
		m_Sphere.Render(m_pCommandList);
	}

	void ASkySphere::OnRender()
	{

	}

	void ASkySphere::Destroy()
	{
	}

	void ASkySphere::OnEvent(Event& e)
	{
	}

	void ASkySphere::BeginPlay()
	{
	}

	void ASkySphere::Tick(const float& deltaMs)
	{
	}

	void ASkySphere::Exit()
	{
	}

	void ASkySphere::OnImGuiRender()
	{
	}

	void Sphere::Init(float radius, int slices, int segments)
	{
		m_Radius = radius;
		m_Slices = slices;
		m_Segments = segments;

		resourceSetup();
	}

	void Sphere::resourceSetup()
	{
		Direct3D12Context& graphicsContext = Direct3D12Context::Get();
		ID3D12Device* pDevice = &graphicsContext.GetDeviceContext();
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
		//int* triangles = new int[nbIndexes];


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
		//Create D3D resources
		D3D12_HEAP_PROPERTIES heapProperty;
		ZeroMemory(&heapProperty, sizeof(heapProperty));
		heapProperty.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC resourceDesc;
		ZeroMemory(&resourceDesc, sizeof(resourceDesc));
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = 0;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.Width = sizeof(SimpleVertex) * verts.size();
		resourceDesc.Height = 1;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		hr = pDevice->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(m_VertexBuffer.GetAddressOf()));
		ThrowIfFailed(hr, "Faield to create commited resource for skysphere");

		UINT8* dataBegin;
		hr = m_VertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&dataBegin));
		ThrowIfFailed(hr, "Failed to map resource fo GPU");
		memcpy(dataBegin, &verts[0], sizeof(SimpleVertex) * verts.size());
		m_VertexBuffer->Unmap(0, nullptr);

		m_VertexView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
		m_VertexView.StrideInBytes = sizeof(SimpleVertex);
		m_VertexView.SizeInBytes = sizeof(SimpleVertex) * (UINT)verts.size();

		resourceDesc.Width = sizeof(int) * triangles.size();
		hr = pDevice->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(m_IndexBuffer.GetAddressOf()));
		ThrowIfFailed(hr, "Failed to create commited resource for skysphere");
		m_IndexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&dataBegin));
		ThrowIfFailed(hr, "Failed to map commited resource to GPU");
		memcpy(dataBegin, &triangles[0], sizeof(int) * triangles.size());
		m_IndexBuffer->Unmap(0, nullptr);


		m_IndexView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
		m_IndexView.Format = DXGI_FORMAT_R32_UINT;
		m_IndexView.SizeInBytes = sizeof(int) * (UINT)triangles.size();
	}

	void Sphere::Render(ComPtr<ID3D12GraphicsCommandList> commandList)
	{
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetIndexBuffer(&m_IndexView);
		commandList->IASetVertexBuffers(0, 1, &m_VertexView);
		commandList->DrawIndexedInstanced(m_IndexSize, 1, 0, 0, 0);
	}

}
