-- Helpful premake documentation
-- Tokens https://github.com/premake/premake-core/wiki/Tokens

workspace ("InsightEngine")
	architecture ("x64")
	startproject ("Application_Windows")

	configurations
	{
		"Debug",
		"Release",
		"Engine-Dist",
		"Game-Dist"
	}

outputdir = "%{cfg.buildcfg}-$(SDKIdentifier)-$(Platform)"


CustomDefines = {}
CustomDefines["IE_BUILD_DIR"] = "../Bin/" .. outputdir
CustomDefines["IE_BUILD_CONFIG"] = outputdir

-- Tools
group "Tools"
include "Engine/Third_Party/ImGui/premake5.lua"
group ""

-- Engine
include "Application_Windows/Application-Windows-Make.lua"
include "Engine/Engine-Make.lua"

