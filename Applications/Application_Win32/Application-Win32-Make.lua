-- Application
-- The client exe that gets executed.
include ("../Common-App-Config.lua")


appName = "Application"
platform = "Win32"
projectName = appName .. "_" .. platform
engineRuntime = "EngineBuild_" .. platform

engineThirdPartyDir = "%{wks.location}/Engine/ThirdParty/"

win32AppIncludeDirs = {}
win32AppIncludeDirs["assimp"]					= engineThirdPartyDir .. "assimp-5.0.1/include/"
win32AppIncludeDirs["Microsoft"] 				= engineThirdPartyDir .. "Microsoft/"
win32AppIncludeDirs["Nvidia"]					= engineThirdPartyDir .. "Nvidia/"
win32AppIncludeDirs["rapidjson"] 				= engineThirdPartyDir .. "rapidjson/include/"
win32AppIncludeDirs["Mono"]						= monoInstallDir .. "include/"
win32AppIncludeDirs["DxcAPI"]					= engineThirdPartyDir .. "Microsoft/DxcAPI/"
win32AppIncludeDirs["Engine_Root"]				= "%{wks.location}/Engine/"
win32AppIncludeDirs["Engine_Src"]				= "%{wks.location}/Engine/Source/"
win32AppIncludeDirs["Engine_ThirdParty"]		= engineThirdPartyDir
win32AppIncludeDirs["BuildRules"]				= "%{wks.location}/Engine/BuildRules/"


project (projectName)
	location ("%{wks.location}/Applications/" .. projectName)
	kind ("WindowedApp")
	language ("C++")
	cppdialect ("C++17")
	staticruntime ("off")
	targetname ("%{prj.name}")
	systemversion ("latest")
	
	targetdir (ieGetBuildFolder())
	objdir (ieGetBuildIntFolder())
	
	debugdir (ieGetBuildFolder())

	files
	{
		"Application-Win32-Make.lua",

		-- Personal Source Files for this Application
		"Source/**.h",
		"Source/**.cpp",
		"./**.h",

		-- Windows resource and acceleration files
		"./**.rc",
	}

	includedirs
	{		
		"%{win32AppIncludeDirs.assimp}",
		"%{win32AppIncludeDirs.Microsoft}",
		"%{win32AppIncludeDirs.Nvidia}DirectX12/",
		"%{win32AppIncludeDirs.Microsoft}WinPixEventRuntime/Include/",
		"%{win32AppIncludeDirs.rapidjson}",
		"%{win32AppIncludeDirs.Mono}mono-2.0/",
		"%{win32AppIncludeDirs.DxcAPI}inc/",
		"%{win32AppIncludeDirs.Engine_Src}",
		"%{win32AppIncludeDirs.Engine_ThirdParty}",
		
		-- Personal source files for this application
		"Source/",

		"./",

		-- Shared Header Includes for this Project
		"%{win32AppIncludeDirs.BuildRules}/PCH_Source/",
	}

	links
	{
        -- Third Party
		"MonoPosixHelper.lib",
		"mono-2.0-sgen.lib",
        "libmono-static-sgen.lib",
        
        -- DirectX/Windows API
		"d3d12.lib",
		"dxgi.lib",
		"d3d11.lib",
		"WinPixEventRuntime.lib",
		"Shlwapi.lib",
		"DirectXTK.lib",
        "DirectXTK12.lib",
		"d2d1.lib",
		"dwrite.lib",
		"dxcompiler.lib",
		
		"Shaders",

		-- Set the Runtime Library to Win32
		engineRuntime,
	}

	libdirs
	{
		"%{win32AppIncludeDirs.DxcAPI}lib/x64",
	}

	defines
	{
		"IE_PLATFORM_BUILD_WIN32=1"
	}
	flags
	{
		"MultiProcessorCompile"
	}
	postbuildcommands
	{
		-- Mono
		"%{commonPostBuildCommands.monodll}",

		-- Remove the engine build library to lighten the folder.
		--("Rmdir /Q /S $(TargetDir)" .. engineRuntime)

		-- Delete the duplicate shaders
		"%{commonPostBuildCommands.delShaderDups}",
	}


-- Build Configurations

	filter "configurations:DebugEditor"
		links
		{
			"assimp-vc142-mtd.lib",
		}
		libdirs
		{
            "%{win32AppIncludeDirs.Engine_ThirdParty}/assimp-5.0.1/build/code/Debug/",
            "%{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/WinPixEventRuntime/bin/x64/",
            "%{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Debug/",
            "%{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Debug/",
			monoInstallDir .. "/lib/",
		}
		postbuildcommands
		{
			-- Assimp
			"%{commonPostBuildCommands.assimpdll_debug}",

			-- PIX
			"%{commonPostBuildCommands.PIXRuntimeWin32dll}",
			
			-- Create a virtual copy of the assets folder
			"%{commonPostBuildCommands.debugContentDir}",
		}
	


	filter "configurations:Development or DebugGame"
		links
		{
			"assimp-vc142-mt.lib",
		}
		libdirs
		{
            "%{win32AppIncludeDirs.Engine_ThirdParty}/assimp-5.0.1/build/code/Release/",
            "%{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/WinPixEventRuntime/bin/x64/",
            "%{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release/",
            "%{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release/",
			monoInstallDir .. "/lib",
		}
		postbuildcommands
		{
			-- Assimp
			"%{commonPostBuildCommands.assimpdll_release}",
			
			-- PIX					
			"%{commonPostBuildCommands.PIXRuntimeWin32dll}",

			-- Copy over the assets
			"%{commonPostBuildCommands.releaseContentDir}",
		}
		


	filter "configurations:ShippingGame"
		links
		{
			"assimp-vc142-mt.lib",
		}
		libdirs
		{
			"%{win32AppIncludeDirs.Engine_ThirdParty}/assimp-5.0.1/build/code/Release",            
            "%{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/WinPixEventRuntime/bin/x64",
            "%{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release",
            "%{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release",
			monoInstallDir .. "/lib",
		}
		postbuildcommands
		{
			-- Assimp
			"%{commonPostBuildCommands.assimpdll_release}",

			-- Copy over the assets
			"%{commonPostBuildCommands.releaseContentDir}",
		}

dofile ("../../Engine/BuildRules/Common-Build-Config.lua")
	
