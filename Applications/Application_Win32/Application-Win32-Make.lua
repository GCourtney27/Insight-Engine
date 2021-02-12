-- Application
-- The client exe that gets executed.

appName = "Application"
platform = "Win32"
projectName = appName .. "_" .. platform
engineRuntime = "EngineBuild_" .. platform

engineThirdPartyDir = "%{wks.location}/Engine/ThirdParty/"
monoInstallDir = "C:/Program Files/Mono/"

win32AppIncludeDirs = {}
win32AppIncludeDirs["assimp"]					= engineThirdPartyDir .. "assimp-5.0.1/include/"
win32AppIncludeDirs["Microsoft"] 				= engineThirdPartyDir .. "Microsoft/"
win32AppIncludeDirs["Nvidia"]					= engineThirdPartyDir .. "Nvidia/"
win32AppIncludeDirs["rapidjson"] 				= engineThirdPartyDir .. "rapidjson/include/"
win32AppIncludeDirs["Mono"]						= monoInstallDir .. "include/"
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
	
	targetdir (ieGetBuildFolder(platform))
	objdir (ieGetBuildIntFolder(platform))
	debugdir ("%{cfg.targetdir}/")

	files
	{
		"Application-Win32-Make.lua",

		-- Personal Source Files for this Application
		"Source/**.h",
		"Source/**.cpp",
		"./**.h",

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
		"assimp-vc142-mtd.lib",
        
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
		
		-- Set the Runtime Library to Win32
		engineRuntime,
	}

	defines
	{
		"IE_PLATFORM_BUILD_WIN32=1",
	}
	flags
	{
		"MultiProcessorCompile"
	}
	postbuildcommands
	{
		-- Assimp
		("{COPY} %{win32AppIncludeDirs.Engine_ThirdParty}/assimp-5.0.1/build/code/Debug/assimp-vc142-mtd.dll %{cfg.targetdir}"),
		-- DX11 Debug Layers
		("{COPY} %{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX11/Bin/D3D11SDKLayers.dll %{cfg.targetdir}"),
		("{COPY} %{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX11/Bin/D3DX11d_43.dll %{cfg.targetdir}"),
		("{COPY} %{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX11/Bin/D3D11Ref.dll %{cfg.targetdir}"),
		-- Mono
		("{COPY} \"".. monoInstallDir .."/bin/mono-2.0-sgen.dll\" %{cfg.targetdir}"),
		-- PIX
		("{COPY} %{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/WinPixEventRuntime/bin/x64/WinPixEventRuntime.dll %{cfg.targetdir}"),
		-- Copy over assets
		("{COPY} %{wks.location}Content %{cfg.targetdir}/../Content"),
		-- Copy over default engine assets
		("{COPY} ../../Engine/Assets %{cfg.targetdir}/../Content/Engine"),
	}


-- Build Configurations
--
	filter "configurations:*Debug"
		defines ("IE_DEBUG")
		symbols ("On")
		runtime ("Debug")
		libdirs
		{
            "%{win32AppIncludeDirs.Engine_ThirdParty}/assimp-5.0.1/build/code/Debug/",
            "%{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/WinPixEventRuntime/bin/x64/",
            "%{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Debug/",
            "%{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Debug/",
			monoInstallDir .. "/lib/",
		}
	


	filter "configurations:*Release"
		defines { "IE_RELEASE", "IE_DEBUG" }
		symbols ("on")
		runtime ("Release")
		optimize ("on")
		libdirs
		{
			"%{win32AppIncludeDirs.Engine_ThirdParty}/assimp-5.0.1/build/code/Release/",
            "%{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/WinPixEventRuntime/bin/x64/",
            "%{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release/",
			"%{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX12/DXTex/DirectXTex/Bin/Desktop_2019_Win10/x64/Release/",
            "%{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release/",
			monoInstallDir .. "/lib",
		}
		postbuildcommands
		{
			-- Assimp
			("{COPY} %{win32AppIncludeDirs.Engine_ThirdParty}/assimp-5.0.1/build/code/Release/assimp-vc140-mt.dll %{cfg.targetdir}"),
			-- DX11 Debug Layers	
			("{COPY} %{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX11/Bin/D3D11SDKLayers.dll %{cfg.targetdir}"),
			("{COPY} %{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX11/Bin/D3DX11d_43.dll %{cfg.targetdir}"),
			("{COPY} %{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX11/Bin/D3D11Ref.dll %{cfg.targetdir}"),
			-- Mono					
			("{COPY} %{win32AppIncludeDirs.Engine_ThirdParty}/Mono/bin/mono-2.0-sgen.dll %{cfg.targetdir}"),
			-- DirectX				
			("{COPY} %{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX12/Bin/dxcompiler.dll %{cfg.targetdir}"),
			("{COPY} %{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX12/Bin/dxil.dll %{cfg.targetdir}"),
			("{COPY} %{wks.location}/Engine/Shaders/HLSL/Ray_Tracing/** ../Binaries/" .. outputdir.."/Engine"),
			-- PIX					
			("{COPY} %{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/WinPixEventRuntime.dll %{cfg.targetdir}"),
			("{COPY} %{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/WinPixEventRuntime_UAP.dll %{cfg.targetdir}"),
			-- Copy over assets
			("{COPY} $(USERPROFILE)/Documents/Insight-Projects/Development-Project/Content/** ../Binaries/" .. outputdir .. "/Content"),
			("{COPY} %{wks.location}/Engine/Assets/Textures/Default_Object/** ../Binaries/"..outputdir.."/Content/Default_Assets/")
		}
		
	filter "configurations:GameDist"
		defines "IE_GAME_DIST"
		optimize "on"
		symbols "on"
		libdirs
		{
			"%{win32AppIncludeDirs.Engine_ThirdParty}/assimp-5.0.1/build/code/Release",            
            "%{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/WinPixEventRuntime/bin/x64",
            "%{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release",
			"%{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX12/DXTex/DirectXTex/Bin/Desktop_2019_Win10/x64/Release",
            "%{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release",
			monoInstallDir .. "/lib",
		}
		postbuildcommands
		{
			-- assimp
			("{COPY} %{win32AppIncludeDirs.Engine_ThirdParty}/assimp-5.0.1/build/code/Release/assimp-vc140-mt.dll %{cfg.targetdir}"),
			-- mono
			("{COPY} %{win32AppIncludeDirs.Engine_ThirdParty}/Mono/bin/mono-2.0-sgen.dll %{cfg.targetdir}"),
			-- DirectX
			("{COPY} %{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX12/Bin/dxcompiler.dll %{cfg.targetdir}"),
			("{COPY} %{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX12/Bin/dxil.dll %{cfg.targetdir}"),
			("{COPY} %{wks.location}/Engine/Shaders/HLSL/Ray_Tracing/** ../Binaries/" .. outputdir.."/Engine"),
			("{COPY} %{wks.location}/Engine/Assets/Textures/Default_Object/** ../Binaries/"..outputdir.."/Content/Default_Assets/")
		}

	