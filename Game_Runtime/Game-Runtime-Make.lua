-- Engine_Source
-- Engine_Source project is a container for the source code. It does not build anything.

rootDirPath = "../"
engineThirdPartyDir = rootDirPath .. "Engine_Source/Third_Party/"
projectName = "Game_Runtime"

gameRuntimeIncludeDirs = {}
gameRuntimeIncludeDirs["Engine_Source_Root"]		= rootDirPath .. "Engine_Source/"
gameRuntimeIncludeDirs["Engine_Source_Src"]			= rootDirPath .. "Engine_Source/Source/"
gameRuntimeIncludeDirs["Engine_Source_Third_Party"] = rootDirPath .. "Engine_Source/Third_Party/"

project (projectName)
	location (rootDirPath .. projectName)
	kind ("SharedLib")
	language ("C++")
	cppdialect ("C++17")

	targetdir (rootDirPath .. binaryDirectory .. "%{prj.name}/Staging")
    objdir (rootDirPath .. intDirectory .. "%{prj.name}")

	files
	{
		"Game-Runtime-Make.lua",

		-- Personal Source Files for this Application
		"Source/**.h",
		"Source/**.cpp",
		"./**.h",
		"./**.cpp",

	}

	defines
	{
	}

	includedirs
	{
		-- Engine source
		"%{gameRuntimeIncludeDirs.Engine_Source_Src}/",
		
		-- Personal Source Files for this game
		"Source/",
		"./",
	}

	filter "configurations:not *Package"
	defines { "BUILD_GAME_DLL", "COMPILE_DLL" }
