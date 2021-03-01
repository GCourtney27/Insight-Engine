#include <Engine_pch.h>

#include "ASkySphere.h"

#include "Runtime/GameFramework/Components/ActorComponent.h"
#include "Runtime/GameFramework/Components/StaticMeshComponent.h"

#include "Platform/DirectX12/Wrappers/D3D12Texture.h"
#include "Platform/DirectX11/Wrappers/D3D11Texture.h"
#include "Platform/DirectX12/Direct3D12Context.h"

namespace Insight {



	ASkySphere::ASkySphere(ActorId id, GameFramework::ActorType type)
		: AActor(id, type)
	{
		Renderer::CreateSkybox();
		Renderer::RegisterSkySphere(this);
	}

	ASkySphere::~ASkySphere()
	{
		Destroy();
	}

	bool ASkySphere::LoadFromJson(const rapidjson::Value* jsonSkySphere)
	{
		std::string diffuseMap;
		const rapidjson::Value& sky = (*jsonSkySphere)["Sky"];
		json::get_string(sky[0], "Diffuse", diffuseMap);

		

		Texture::IE_TEXTURE_INFO diffuseInfo;
		diffuseInfo.Filepath = FileSystem::GetRelativeContentDirectoryW(StringHelper::StringToWide(diffuseMap));
		diffuseInfo.Type = Texture::ETextureType::TT_SkyDiffuse;
		diffuseInfo.GenerateMipMaps = true;
		diffuseInfo.IsCubeMap = true;
		
		switch (Renderer::GetAPI())
		{
		case Renderer::ETargetRenderAPI::Direct3D_11:
		{
			m_Diffuse = new ieD3D11Texture(diffuseInfo);
			break;
		}
		case Renderer::ETargetRenderAPI::Direct3D_12:
		{
			Direct3D12Context& RenderContext = Renderer::GetAs<Direct3D12Context>();
			CDescriptorHeapWrapper& cbvSrvheap = RenderContext.GetCBVSRVDescriptorHeap();
			m_Diffuse = new ieD3D12Texture(diffuseInfo, cbvSrvheap);
			break;
		}	
		}

		return true;
	}

	bool ASkySphere::WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>* Writer)
	{
		Writer->StartObject(); // Start Write Actor
		{
			Writer->Key("Type");
			Writer->String("SkySphere");

			Writer->Key("DisplayName");
			Writer->String(StringHelper::WideToString(SceneNode::GetDisplayName()).c_str());

			Writer->Key("Transform");
			Writer->StartArray(); // Start Write Transform
			{
				//ieTransform& Transform = SceneNode::GetTransformRef();
				//ieVector3 Pos = Transform.GetPosition();
				//ieVector3 Rot = Transform.GetRotation();
				//ieVector3 Sca = Transform.GetScale();

				//Writer->StartObject();
				//// Position
				//Writer->Key("posX");
				//Writer->Double(Pos.x);
				//Writer->Key("posY");
				//Writer->Double(Pos.y);
				//Writer->Key("posZ");
				//Writer->Double(Pos.z);
				//// Rotation
				//Writer->Key("rotX");
				//Writer->Double(Rot.x);
				//Writer->Key("rotY");
				//Writer->Double(Rot.y);
				//Writer->Key("rotZ");
				//Writer->Double(Rot.z);
				//// Scale
				//Writer->Key("scaX");
				//Writer->Double(Sca.x);
				//Writer->Key("scaY");
				//Writer->Double(Sca.y);
				//Writer->Key("scaZ");
				//Writer->Double(Sca.z);

				Writer->EndObject();
			}
			Writer->EndArray(); // End Write Transform

			// Sky Attributes
			Writer->Key("Sky");
			Writer->StartArray();
			{
				Writer->StartObject();
				Writer->Key("Diffuse");
				Writer->String(StringHelper::WideToString(m_Diffuse->GetFilepath()).c_str());
				Writer->EndObject();
			}
			Writer->EndArray();

			Writer->Key("Subobjects");
			Writer->StartArray(); // Start Write SubObjects
			{
				for (size_t i = 0; i < m_NumComponents; ++i)
				{
					AActor::m_Components[i]->WriteToJson(*Writer);
				}
			}
			Writer->EndArray(); // End Write SubObjects
		}
		Writer->EndObject(); // End Write Actor
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
