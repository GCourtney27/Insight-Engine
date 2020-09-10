#pragma once

#include <Insight/Core.h>

namespace Insight {

	using Microsoft::WRL::ComPtr;

	class Texture
	{
	public:
		typedef uint32_t ID;

		enum eTextureType
		{
			eTextureType_Invalid = -1,
			// Per Object
			eTextureType_Albedo = 0,
			eTextureType_Normal = 1,
			eTextureType_Roughness = 2,
			eTextureType_Metallic = 3,
			eTextureType_AmbientOcclusion = 4,
			eTextureType_Opacity = 5,
			eTextureType_Translucency = 6,
			// Sky Sphere
			eTextureType_SkyIrradience = 7,
			eTextureType_SkyEnvironmentMap = 8,
			eTextureType_IBLBRDFLUT = 9,
			eTextureType_SkyDiffuse = 10,
		};

		struct IE_TEXTURE_INFO
		{
			eTextureType Type = eTextureType::eTextureType_Invalid;
			bool GenerateMipMaps = true;
			bool IsCubeMap = false;
			std::wstring Filepath;
			std::string AssetDirectoryRelPath;
			std::string DisplayName;
			ID Id;
		};

	public:
		Texture(IE_TEXTURE_INFO createInfo);
		Texture() {}
		virtual ~Texture();
	
		bool operator==(const Texture& Tex)
		{
			return m_TextureInfo.Id == Tex.GetTextureInfo().Id;
		}

		// Destroy and release texture resources.
		virtual void Destroy() = 0;
		// Binds the texture to the pipeline to be drawn in the scene pass.
		virtual void BindForDeferredPass() = 0;
		virtual void BindForForwardPass() = 0;
		
		// Get the general information about this texture.
		inline const IE_TEXTURE_INFO& GetTextureInfo() const { return m_TextureInfo; }
		// Get the filename for this texture.
		inline const std::string& GetDisplayName() const { return m_TextureInfo.DisplayName; }
		// Get the full file path to this texture on disk.
		inline const std::wstring& GetFilepath() const { return m_TextureInfo.Filepath; }
		// Get the Asset directory relative path for the texture for this project.
		inline const std::string& GetAssetDirectoryRelPath() const { return m_TextureInfo.AssetDirectoryRelPath; }

	protected:
		IE_TEXTURE_INFO				m_TextureInfo = {};
	};

	using IE_TEXTURE_INFO = Texture::IE_TEXTURE_INFO;

}