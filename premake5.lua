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
		-- Debug symbols for engine activated. Optimal configuration for
		-- adding and testing new engine features.
		"DebugEditor",

		-- Debug symbols for engine activated and able to open
		-- projects and edit worlds.
		"Development",
		
		-- Optomized engine code with debug symbols for game code.
		-- Builds a standalone exe that can be luanched, and the game played.
		"DebugGame",

		-- Full engine and game code optomizations applied.
		-- Builds a standalone exe that can be luanched, and the game played.
		"ShippingGame"
	}
	

	platforms
	{
		-- An app that runs using the Win32 API.
		"Win32Desktop",

		-- An app that runs on Xbox One platforms
		"XboxOne",
		
		-- An app that runs on the Windows Runtime (UWP)
		"UniversalWindowsDesktop",
	}



outputdir = "%{cfg.buildcfg}-$(SDKIdentifier)-$(Platform)"

binaryFolder = "Binaries/"
intFolder = binaryFolder .. "Intermediates/"

function ieGetBuildFolder()
	return "%{wks.location}/" .. binaryFolder .. "/%{cfg.buildcfg}-%{cfg.platform}-%{cfg.architecture}/"
end

function ieGetBuildIntFolder()
	return "%{wks.location}/" .. intFolder .. "/%{cfg.buildcfg}-%{cfg.platform}-%{cfg.architecture}/%{prj.name}"
end


-- Tools
group ("Tools")
	include ("Engine/ThirdParty/ImGui/premake5.lua")
group ("")

-- Applications
group ("Applications")
	include ("Applications/Application_Win32/Application-Win32-Make.lua")
	include ("Applications/Application_UWP/Application-UWP-Make.lua")
group ("")

-- Engine
include ("Engine/Engine-Make.lua")

--Shaders
include ("Engine/Shaders/Shaders-Make.lua")

-- Engine Source Build Rules
group ("Build Rules")
	include ("Engine/BuildRules/Build-UWP-Make.lua")
	include ("Engine/BuildRules/Build-Win32-Make.lua")
group ("")
