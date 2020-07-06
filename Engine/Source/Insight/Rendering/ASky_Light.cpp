#include <ie_pch.h>

#include "ASky_Light.h"

#include "Insight/Runtime//Components/Actor_Component.h"
#include "Platform/DirectX12/Direct3D12_Context.h"
#include "Insight/Systems/File_System.h"

namespace Insight {



	ASkyLight::ASkyLight(ActorId id, ActorType type)
		: AActor(id, type)
	{
		Direct3D12Context& graphicsContext = Direct3D12Context::Get();
		m_pCommandList = &graphicsContext.GetScenePassCommandList();
		graphicsContext.AddSkyLight(this);
	}

	ASkyLight::~ASkyLight()
	{
	}

	bool ASkyLight::LoadFromJson(const rapidjson::Value& jsonSkyLight)
	{
		std::string brdfLUT, irMap, envMap;
		const rapidjson::Value& sky = jsonSkyLight["Sky"];
		json::get_string(sky[0], "BRDFLUT", brdfLUT);
		json::get_string(sky[0], "Irradiance", irMap);
		json::get_string(sky[0], "Environment", envMap);

		CDescriptorHeapWrapper& cbvSrvheap = Direct3D12Context::Get().GetCBVSRVDescriptorHeap();

		Texture::IE_TEXTURE_INFO brdfInfo;
		brdfInfo.Filepath = StringHelper::StringToWide(FileSystem::Get().GetRelativeAssetDirectoryPath(brdfLUT));
		brdfInfo.Type = Texture::eTextureType::SKY_BRDF_LUT;
		brdfInfo.IsCubeMap = true;
		brdfInfo.GenerateMipMaps = false;
		m_BrdfLUT.Init(brdfInfo, cbvSrvheap);

		Texture::IE_TEXTURE_INFO irMapInfo;
		irMapInfo.Filepath = StringHelper::StringToWide(FileSystem::Get().GetRelativeAssetDirectoryPath(irMap));
		irMapInfo.Type = Texture::eTextureType::SKY_IRRADIENCE;
		irMapInfo.IsCubeMap = true;
		brdfInfo.GenerateMipMaps = false;
		m_Irradiance.Init(irMapInfo, cbvSrvheap);
		
		Texture::IE_TEXTURE_INFO envMapInfo;
		envMapInfo.Filepath = StringHelper::StringToWide(FileSystem::Get().GetRelativeAssetDirectoryPath(envMap));
		envMapInfo.Type = Texture::eTextureType::SKY_ENVIRONMENT_MAP;
		envMapInfo.IsCubeMap = true;
		brdfInfo.GenerateMipMaps = false;
		m_Environment.Init(envMapInfo, cbvSrvheap);

		return true;
	}

	bool ASkyLight::WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer)
	{
		Writer.StartObject(); // Start Write Actor
		{
			Writer.Key("Type");
			Writer.String("SkyLight");

			Writer.Key("DisplayName");
			Writer.String(SceneNode::GetDisplayName());

			Writer.Key("Transform");
			Writer.StartArray(); // Start Write Transform
			{
				Transform& Transform = SceneNode::GetTransformRef();
				Vector3 Pos = Transform.GetPosition();
				Vector3 Rot = Transform.GetRotation();
				Vector3 Sca = Transform.GetScale();

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
				Writer.Key("BRDFLUT");
				Writer.String(StringHelper::WideToString(m_BrdfLUT.GetFilepath()).c_str());
				Writer.Key("Irradiance");
				Writer.String(StringHelper::WideToString(m_Irradiance.GetFilepath()).c_str());
				Writer.Key("Environment");
				Writer.String(StringHelper::WideToString(m_Environment.GetFilepath()).c_str());
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

	bool ASkyLight::OnInit()
	{
		return false;
	}

	bool ASkyLight::OnPostInit()
	{
		return false;
	}

	void ASkyLight::OnUpdate(const float& deltaMs)
	{
	}

	void ASkyLight::OnPreRender(XMMATRIX parentMat)
	{
	}

	void ASkyLight::OnRender()
	{
		m_Environment.Bind();
		m_Irradiance.Bind();
		m_BrdfLUT.Bind();
	}

	void ASkyLight::Destroy()
	{
	}

	void ASkyLight::OnEvent(Event& e)
	{
	}

	void ASkyLight::BeginPlay()
	{
	}

	void ASkyLight::Tick(const float& deltaMs)
	{
	}

	void ASkyLight::Exit()
	{
	}

	void ASkyLight::OnImGuiRender()
	{
	}

}
