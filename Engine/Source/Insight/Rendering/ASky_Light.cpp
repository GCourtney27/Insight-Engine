#include <ie_pch.h>

#include "Platform/DirectX12/Direct3D12_Context.h"
#include "Insight/Systems/File_System.h"

#include "ASky_Light.h"

namespace Insight {



	ASkyLight::ASkyLight(ActorId id, ActorType type)
		: AActor(id, type)
	{
		Direct3D12Context& graphicsContext = Direct3D12Context::Get();
		m_pCommandList = &graphicsContext.GetCommandList();
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
