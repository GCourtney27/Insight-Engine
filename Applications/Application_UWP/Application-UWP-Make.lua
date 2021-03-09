-- UWP Application

include ("../Common-App-Config.lua")


appName = "Application"
platform = "UWP"
projectName = appName .. "_" .. platform
engineRuntime = "EngineBuild_" .. platform

rootDirPath = "../../"
engineThirdPartyDir = rootDirPath .. "Engine/ThirdParty/"

uwpAppIncludeDirs = {}
uwpAppIncludeDirs["tinyobjloader"]			= engineThirdPartyDir .. "tinyobjloader/include/"
uwpAppIncludeDirs["OpenFBX"]				= engineThirdPartyDir .. "OpenFBX/src/"
uwpAppIncludeDirs["DxcAPI"]					= engineThirdPartyDir .. "Microsoft/DxcAPI/"
uwpAppIncludeDirs["Microsoft"] 				= engineThirdPartyDir .. "Microsoft/"
uwpAppIncludeDirs["Nvidia"]					= engineThirdPartyDir .. "Nvidia/"
uwpAppIncludeDirs["rapidjson"] 				= engineThirdPartyDir .. "rapidjson/include/"
uwpAppIncludeDirs["Engine_Root"]			= rootDirPath .. "Engine/"
uwpAppIncludeDirs["Engine_Src"]				= rootDirPath .. "Engine/Source/"
uwpAppIncludeDirs["Engine_ThirdParty"]		= rootDirPath .. "Engine/ThirdParty/"
uwpAppIncludeDirs["BuildRules"]				= rootDirPath .. "Engine/BuildRules/"

project (projectName)
	location (rootDirPath.. "Applications/" .. projectName)
	kind ("WindowedApp")
	language ("C++")
	cppdialect ("C++17")
	staticruntime ("off")
	systemversion ("10.0.18362.0")
	targetname (projectName)
	
	targetdir (ieGetBuildFolder())
	objdir (ieGetBuildIntFolder())

	debugdir ("%{wks.location}/Engine")

	platforms { "x64" }
	defaultlanguage ("en-US")
	system ("windowsuniversal")
	consumewinrtextension ("false")
	generatewinmd ("false")
	certificatefile ("Application_UWP_WinRT_TemporaryKey.pfx")
	certificatethumbprint ("cda81d56ffb83d59cc6f6a105f3b91c51fbc6e99")
	
	pchheader ("pch.h")
	pchsource ("Source/pch.cpp")


	files
	{
		"Application-UWP-Make.lua",

		-- Deployment Assets
        "Assets/*.png",
        "Package.appxmanifest",
		"Application_UWP_WinRT_TemporaryKey.pfx",

		-- Personal Source Files for this Application
		"Source/**.h",
		"Source/**.cpp",
	}

	includedirs
	{
		"%{uwpAppIncludeDirs.OpenFBX}",
		"%{uwpAppIncludeDirs.tinyobjloader}",
		"%{uwpAppIncludeDirs.Microsoft}",
		"%{uwpAppIncludeDirs.Microsoft}/DirectX12",
		"%{uwpAppIncludeDirs.Nvidia}DirectX12/",
		"%{uwpAppIncludeDirs.Microsoft}/WinPixEventRuntime/Include/",
		"%{uwpAppIncludeDirs.rapidjson}",
		"%{uwpAppIncludeDirs.Engine_Src}/",
		"%{uwpAppIncludeDirs.Engine_ThirdParty}/",
		
		-- Personal Source Files for this Application
		"Source/",

		-- Shared Header Includes for this Project
		"%{uwpAppIncludeDirs.BuildRules}/PCH_Source/",
	}

	links
	{     
        -- DirectX/Windows API
		"d3d12.lib",
		"dxgi.lib",
		"d3d11.lib",
		"DirectXTK.lib",
		"D3Dcompiler.lib",
		"DirectXTK12.lib",
		"d2d1.lib",
		"dwrite.lib",
		
		"Shaders",
		
		-- Set the engine library to UWP
		"EngineBuild_UWP",
	}

	libdirs
	{
		"%{uwpAppIncludeDirs.DxcAPI}lib/x64",
	}

	defines
	{
		"IE_PLATFORM_BUILD_UWP=1",
	}

	flags
	{
		"MultiProcessorCompile"
	}

	postbuildcommands
	{
	}

		
-- Build Configurations

	filter "configurations:DebugEditor"
		libdirs
		{
			"%{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/WinPixEventRuntime/bin/x64",
			"%{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Debug/",
			"%{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Debug/",
			monoInstallDir .. "/lib/",
		}
		postbuildcommands
		{
			-- PIX
			"%{commonPostBuildCommands.PIXRuntimeUWPdll}",

			-- Create a virtual copy of the assets folder
			"%{commonPostBuildCommands.debugContentDir}",
			
			-- Delete the duplicate shaders
			"%{commonPostBuildCommands.delShaderDups}",
		}



	filter "configurations:Development or DebugGame"
		libdirs
		{
			"%{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/WinPixEventRuntime/bin/x64",
			"%{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release",
			"%{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release",
			monoInstallDir .. "/lib",
		}
		postbuildcommands
		{
			-- PIX					
			"%{commonPostBuildCommands.PIXRuntimeUWPdll}",
		
			-- Copy over the assets
			"%{commonPostBuildCommands.releaseContentDir}",
		}

	filter "configurations:ShippingGame"
		libdirs
		{       
			"%{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/WinPixEventRuntime/bin/x64",
			"%{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release",
			"%{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release",
			monoInstallDir .. "/lib",
		}
		postbuildcommands
		{
			-- Copy over the assets
			"%{commonPostBuildCommands.releaseContentDir}",
		}

		
-- Filters

	filter {"platforms:XboxOne"}
	defines
	{
		"IE_PLATFORM_BUILD_XBOX_ONE=1",
	}

	filter {"platforms:UniversalWindowsDesktop"}
	links
	{
		"WinPixEventRuntime_UAP.lib",
		"dxcompiler.lib",
	}
	

dofile ("../../Engine/BuildRules/Common-Build-Config.lua")
