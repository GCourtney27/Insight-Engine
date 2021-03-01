-- UWP Application

include ("../Common-App-Config.lua")


appName = "Application"
platform = "UWP"
projectName = appName .. "_UWP_WinRT"

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
	systemversion ("latest")
	targetname (projectName)
	
	targetdir (ieGetBuildFolder(platform))
	objdir (ieGetBuildIntFolder(platform))

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
		-- "%{uwpAppIncludeDirs.assimp}",
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
		"WinPixEventRuntime_UAP.lib",
		"DirectXTK.lib",
		"D3Dcompiler.lib",
		"dxcompiler.lib",
		"DirectXTK12.lib",
		"d2d1.lib",
		"dwrite.lib",
		
		-- Set the engine library to UWP
		"EngineBuild_UWP",
	}

	libdirs
	{
		"%{uwpAppIncludeDirs.DxcAPI}lib/x64",
	}
	defines
	{
		-- Compile for UWP platform
		"IE_PLATFORM_BUILD_UWP=1",
	}
	flags
	{
		"MultiProcessorCompile"
	}
	postbuildcommands
	{
		-- DirectX
		"%{commonPostBuildCommands.dxildll}",
		"%{commonPostBuildCommands.dxcompilerdll}",
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

		-- Create a virtual copy of the shaders folder
		"%{commonPostBuildCommands.debugShaderDir}",
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

		-- Copy over the shaders
		"%{commonPostBuildCommands.releaseShaderDir}",
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
		-- mono
		("{COPY} %{win32AppIncludeDirs.Engine_ThirdParty}/Mono/bin/mono-2.0-sgen.dll %{cfg.targetdir}"),

		-- Copy over the assets
		"%{commonPostBuildCommands.releaseContentDir}",

		-- Copy over the shaders
		"%{commonPostBuildCommands.releaseShaderDir}",
	}

dofile ("../../Engine/BuildRules/Common-Build-Config.lua")
