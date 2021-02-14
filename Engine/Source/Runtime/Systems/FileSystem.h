#pragma once

#include <Runtime/CoreMacros.h>
#include "Runtime/Rendering/Renderer.h"

namespace Insight {

	class Scene;

	class INSIGHT_API FileSystem
	{
	public:
		FileSystem();
		~FileSystem();

		/*
			Initializes the filesystems working directory. Should be called once
			during app initialization.
		*/
		static bool Init();

		/*
			Save the Engine settings out to cached file in "Content/" directory
			@param Settings - Settings to write to disk.
		*/
		static void SaveEngineUserSettings(const Renderer::GraphicsSettings& Settings);

		/*
			Get the directory for where the applications content (ie. textures, models etc.) is stored.		
			Ex) Texture.png		
			returns		
			Content/Textures/Texture.png
			@param Path - Path to chain to the return value
		*/
		static std::wstring GetRelativeContentDirectoryW(const std::wstring& Path);

		/*
			Loads the settings the renderer will use during execution. Texture resolution whether 
			ray tracing is enabled etc.
		*/
		static Renderer::GraphicsSettings LoadGraphicsSettingsFromJson();

		/*
			loads a scene from a json file.
			@param Filename - Content directory relative path to the scene to be loaded.
			@pram pScene - Scene object to populate.
		*/
		static bool LoadSceneFromJson(const std::string& FileName, Scene* pScene);

		/*
			Saves a scene to a json file.
			@param pScene - The scene onject to parse to disk.
		*/
		static bool WriteSceneToJson(Scene* pScene);
		
		/*
			Returns true if a file exists in the content directory. False if not.
			@param Path - Content directory relative path of the file to query.
		*/
		static bool FileExistsInContentDirectory(const std::string& Path);
				
		/*
			Returns the path to the pre-compiled code for a shader as an UNICODE string.
			@param Shader - Name of the shader to read.
		*/
		static std::wstring GetShaderPathW(const wchar_t* Shader);

		/*
			Reads in the raw data for a file and returns its contents. nullptr if the file read was unsuccessful. WARNING: Creates data on the heap,
			make sure to free the return value once you are finished with it.
			@param Path - Exe relative path to the file to read.
			@param OutDataSize - Populated with the size of the file that is read in. -1 if the file read was unsuccessful.
		*/
		static char* ReadRawData(const char* Path, size_t& OutDataSize);

	protected:
	};

}
