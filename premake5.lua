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
	


outputdir = "%{cfg.buildcfg}-$(SDKIdentifier)-$(Platform)"

binaryFolder = "Binaries/"
intFolder = binaryFolder .. "Intermediates/"

function ieGetBuildFolder(platform)
	return "%{wks.location}/" .. binaryFolder .. "/%{cfg.buildcfg}-" .. platform .. "-%{cfg.architecture}/"
end

function ieGetBuildIntFolder(platform)
	return "%{wks.location}/" .. intFolder .. "/%{cfg.buildcfg}-" .. platform .. "-%{cfg.architecture}/%{prj.name}"
end


-- Tools
group ("Tools")
	include ("Engine/ThirdParty/ImGui/premake5.lua")
group ("")

-- Applications
group ("Applications")
	include ("Applications/Application_Win32/Application-Win32-Make.lua")
	include ("Applications/Application_UWP_WinRT/Application-UWP-Make.lua")
group ("")

-- Engine
include ("Engine/Engine-Make.lua")

-- Engine Source Build Rules
group ("Build Rules")
	include ("Engine/BuildRules/Build-UWP-Make.lua")
	include ("Engine/BuildRules/Build-Win32-Make.lua")
--	include ("Engine/BuildRules/Build-XboxOne-Make.lua") --TODO
group ("")
