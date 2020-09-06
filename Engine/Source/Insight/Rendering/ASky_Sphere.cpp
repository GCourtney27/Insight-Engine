#include <Engine_pch.h>

#include "ASky_Sphere.h"

#include "Insight/Runtime/Components/Actor_Component.h"
#include "Insight/Runtime/Components/Static_Mesh_Component.h"

#include "Platform/Windows/DirectX_12/ie_D3D12_Texture.h"
#include "Platform/Windows/DirectX_11/ie_D3D11_Texture.h"
#include "Platform/Windows/DirectX_12/Direct3D12_Context.h"

#include "Insight/Systems/File_System.h"

namespace Insight {



	ASkySphere::ASkySphere(ActorId id, ActorType type)
		: AActor(id, type)
	{
		Renderer::CreateSkybox();
		Renderer::RegisterSkySphere(this);
	}

	ASkySphere::~ASkySphere()
	{
		Destroy();
	}

	bool ASkySphere::LoadFromJson(const rapidjson::Value& jsonSkySphere)
	{
		std::string diffuseMap;
		const rapidjson::Value& sky = jsonSkySphere["Sky"];
		json::get_string(sky[0], "Diffuse", diffuseMap);

		

		Texture::IE_TEXTURE_INFO diffuseInfo;
		diffuseInfo.Filepath = StringHelper::StringToWide(FileSystem::GetProjectRelativeAssetDirectory(diffuseMap));
		diffuseInfo.AssetDirectoryRelPath = diffuseMap;
		diffuseInfo.Type = Texture::eTextureType::eTextureType_SkyDiffuse;
		diffuseInfo.GenerateMipMaps = true;
		diffuseInfo.IsCubeMap = true;
		
		switch (Renderer::GetAPI())
		{
		case Renderer::eTargetRenderAPI::D3D_11:
		{
			m_Diffuse = new ieD3D11Texture(diffuseInfo);
			break;
		}
		case Renderer::eTargetRenderAPI::D3D_12:
		{
			Direct3D12Context* graphicsContext = reinterpret_cast<Direct3D12Context*>(&Renderer::Get());
			CDescriptorHeapWrapper& cbvSrvheap = graphicsContext->GetCBVSRVDescriptorHeap();
			m_Diffuse = new ieD3D12Texture(diffuseInfo, cbvSrvheap);
			break;
		}	
		}

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
				ieTransform& Transform = SceneNode::GetTransformRef();
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
				Writer.String(m_Diffuse->GetAssetDirectoryRelPath().c_str());
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

	void ASkySphere::OnUpdate(const float DeltaMs)
	{
	}

	void ASkySphere::OnPreRender(XMMATRIX parentMat)
	{
	}

	void ASkySphere::RenderSky()
	{
		m_Diffuse->BindForDeferredPass();
		Renderer::RenderSkySphere();
	}

	void ASkySphere::OnRender()
	{
	}

	void ASkySphere::Destroy()
	{
		Renderer::RegisterSkySphere(this);
		delete m_Diffuse;
	}

	void ASkySphere::OnEvent(Event& e)
	{
	}

	void ASkySphere::BeginPlay()
	{
	}

	void ASkySphere::Tick(const float DeltaMs)
	{
	}

	void ASkySphere::Exit()
	{
	}

	void ASkySphere::OnImGuiRender()
	{
	}


}
