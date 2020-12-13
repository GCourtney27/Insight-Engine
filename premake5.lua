-- Helpful premake documentation
-- Tokens https://github.com/premake/premake-core/wiki/Tokens

-- The main entry point for the project build system. 

-- Add the premake module to enable WinRT project building.
require ("vstudio")
dofile  ("Vendor/premake/Modules/winrt/_preload.lua")
require ("Vendor/premake/Modules/winrt")

workspace ("InsightEngine")
	architecture ("x64")
	startproject ("Application_Win32")

	configurations
	{
		"Debug",
		"Release",
		"Engine-Dist",
		"Game-Dist"
	}

outputdir = "%{cfg.buildcfg}-$(SDKIdentifier)-$(Platform)"

CustomDefines = {}
CustomDefines["IE_BUILD_DIR"] = "../Binaries/" .. outputdir
CustomDefines["IE_BUILD_CONFIG"] = outputdir

-- Tools
group ("Tools")
	include ("Engine_Source/Third_Party/ImGui/premake5.lua")
group ("")

-- Applications
group ("Applications")
	include ("Application_Win32/Application-Win32-Make.lua")
	include ("Application_UWP_WinRT/Application-UWP-Make.lua")
group ("")

-- Engine
include ("Engine_Source/Engine-Make.lua")

-- Engine Source Build Rules
group ("Build Rules")
	include ("Build_Rules/Build-Rules-Make.lua")
group ("")
