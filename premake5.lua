-- Helpful premake documentation
-- Tokens https://github.com/premake/premake-core/wiki/Tokens

workspace ("Insight")
	architecture ("x86_64")

	configurations
	{
		"Debug",
		"Release",
		"Engine-Dist",
		"Game-Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


CustomDefines = {}
CustomDefines["IE_BUILD_DIR"] = "../Bin/" .. outputdir
CustomDefines["IE_BUILD_CONFIG"] = outputdir

-- Modules
include "Engine/Vendor/ImGui/premake5.lua"
include "Application/Application-Make.lua"
include "Engine/Engine-Make.lua"
include "Engine/Modules/Renderer/Renderer-Make.lua"