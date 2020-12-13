#include <Engine_pch.h>

#include "ASky_Light.h"

#include "Insight/Runtime/Components/Actor_Component.h"
#include "Insight/Rendering/Renderer.h"

#include "Platform/DirectX_12/Wrappers/D3D12_Texture.h"
#include "Platform/DirectX_11/Wrappers/ie_D3D11_Texture.h"
#include "Platform/DirectX_12/Direct3D12_Context.h"

#include "Insight/UI/UI_Lib.h"


namespace Insight {



	ASkyLight::ASkyLight(ActorId id, Runtime::ActorType type)
		: AActor(id, type)
	{
		Renderer::AddSkyLight(this);
	}

	ASkyLight::~ASkyLight()
	{
	}

	bool ASkyLight::LoadFromJson(const rapidjson::Value* jsonSkyLight)
	{
		std::string brdfLUT, irMap, envMap;
		const rapidjson::Value& sky = (*jsonSkyLight)["Sky"];
		json::get_string(sky[0], "BRDFLUT", brdfLUT);
		json::get_string(sky[0], "Irradiance", irMap);
		json::get_string(sky[0], "Radiance", envMap);



		Texture::IE_TEXTURE_INFO brdfInfo;
		brdfInfo.Filepath = FileSystem::GetRelativeContentDirectoryW(StringHelper::StringToWide(brdfLUT));
		brdfInfo.Type = Texture::eTextureType::eTextureType_IBLBRDFLUT;
		brdfInfo.IsCubeMap = false;
		brdfInfo.GenerateMipMaps = false;

		Texture::IE_TEXTURE_INFO irMapInfo;
		irMapInfo.Filepath = FileSystem::GetRelativeContentDirectoryW(StringHelper::StringToWide(irMap));
		irMapInfo.Type = Texture::eTextureType::eTextureType_SkyIrradience;
		irMapInfo.IsCubeMap = true;
		brdfInfo.GenerateMipMaps = false;

		Texture::IE_TEXTURE_INFO radMapInfo;
		radMapInfo.Filepath = FileSystem::GetRelativeContentDirectoryW(StringHelper::StringToWide(envMap));
		radMapInfo.Type = Texture::eTextureType::eTextureType_SkyRadianceMap;
		radMapInfo.IsCubeMap = true;
		brdfInfo.GenerateMipMaps = false;

		switch (Renderer::GetAPI())
		{
		case Renderer::TargetRenderAPI::Direct3D_11:
		{
			m_BrdfLUT = new ieD3D11Texture(brdfInfo);
			m_Irradiance = new ieD3D11Texture(irMapInfo);
			m_Radiance = new ieD3D11Texture(radMapInfo);
			break;
		}
		case Renderer::TargetRenderAPI::Direct3D_12:
		{
			Direct3D12Context& RenderContext = Renderer::GetAs<Direct3D12Context>();
			CDescriptorHeapWrapper& cbvSrvheap = RenderContext.GetCBVSRVDescriptorHeap();
			m_BrdfLUT = new ieD3D12Texture(brdfInfo, cbvSrvheap);
			m_Irradiance = new ieD3D12Texture(irMapInfo, cbvSrvheap);
			m_Radiance = new ieD3D12Texture(radMapInfo, cbvSrvheap);
			break;
		}
		}

		return true;
	}

	bool ASkyLight::WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>* Writer)
	{
		Writer->StartObject(); // Start Write Actor
		{
			Writer->Key("Type");
			Writer->String("SkyLight");

			Writer->Key("DisplayName");
			Writer->String(SceneNode::GetDisplayName());

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
				Writer->Key("BRDFLUT");
				Writer->String(StringHelper::WideToString(m_BrdfLUT->GetFilepath()).c_str());
				Writer->Key("Irradiance");
				Writer->String(StringHelper::WideToString(m_Irradiance->GetFilepath()).c_str());
				Writer->Key("Radiance");
				Writer->String(StringHelper::WideToString(m_Radiance->GetFilepath()).c_str());
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

	bool ASkyLight::OnInit()
	{
		return false;
	}

	bool ASkyLight::OnPostInit()
	{
		return false;
	}

	void ASkyLight::OnUpdate(const float DeltaMs)
	{
	}

	void ASkyLight::OnPreRender(XMMATRIX parentMat)
	{
	}

	void ASkyLight::OnRender()
	{
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

	void ASkyLight::Tick(const float DeltaMs)
	{
	}

	void ASkyLight::Exit()
	{
	}

	void ASkyLight::OnImGuiRender()
	{
		AActor::OnImGuiRender();
		
		UI::Checkbox("Sky Light Enabled", &m_Enabled);
	}

	void ASkyLight::BindCubeMaps(bool RenderPassIsDeferred)
	{
		if (m_Enabled) {

			if (RenderPassIsDeferred) {
				m_Radiance->BindForDeferredPass();
				m_Irradiance->BindForDeferredPass();
				m_BrdfLUT->BindForDeferredPass();
			}
			else {
				m_Radiance->BindForForwardPass();
				m_Irradiance->BindForForwardPass();
				m_BrdfLUT->BindForDeferredPass();
			}
		}
	}

}
