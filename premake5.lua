-- Helpful premake documentation
-- Tokens https://github.com/premake/premake-core/wiki/Tokens

workspace ("Retina")
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
CustomDefines["RN_BUILD_DIR"] = "../Bin/" .. outputdir
CustomDefines["RN_BUILD_CONFIG"] = outputdir

-- Modules
group "Tools"
include "Engine/Vendor/ImGui/premake5.lua"
group ""

include "Application/Application-Make.lua"
include "Engine/Engine-Make.lua"

group "Subsystems"
include "Engine/Modules/Renderer/Renderer-Make.lua"
group ""