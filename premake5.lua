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
group "Tools"
include "Engine/Vendor/ImGui/premake5.lua"
group ""

include "Application/Application-Make.lua"
include "Engine/Engine-Make.lua"
