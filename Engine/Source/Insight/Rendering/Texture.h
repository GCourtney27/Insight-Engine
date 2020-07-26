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
			INVALID = -1,
			// Per Object
			ALBEDO = 0,
			NORMAL = 1,
			ROUGHNESS = 2,
			METALLIC = 3,
			AO = 4,
			// Sky Sphere
			SKY_IRRADIENCE = 5,
			SKY_ENVIRONMENT_MAP = 6,
			SKY_BRDF_LUT = 7,
			SKY_DIFFUSE = 8,
		};

		struct IE_TEXTURE_INFO
		{
			eTextureType Type = eTextureType::INVALID;
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
	

		// Destroy and release texture resources.
		virtual void Destroy() = 0;
		// Binds the texture to the pipeline to be drawn in the scene pass.
		virtual void Bind() = 0;
		
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

}