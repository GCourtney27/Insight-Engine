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
group ("Tools")
	include ("Engine_Source/Third_Party/ImGui/premake5.lua")
group ("")

-- Engine
include ("Engine_Source/Engine-Make.lua")

-- Engine Source Build Rules
group ("Build Rules")
	include ("Build_Rules/Build-Rules-Make.lua")
group ""
-- Applications
group ("Applications")
	include ("Application_Win32/Application-Win32-Make.lua")

	include ("Application_UWP/Application-UWP-Make.lua")

group ("")
