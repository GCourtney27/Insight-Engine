-- Application
-- The client exe that gets executed.

appName = "Application"
projectName = appName .. "_Win32"

engineThirdPartyDir = "../Engine_Source/Third_Party/"
monoInstallDir = "C:/Program Files/Mono/"
rootDirPath = "../"

win32AppIncludeDirs = {}
win32AppIncludeDirs["assimp"]					= engineThirdPartyDir .. "assimp-5.0.1/include/"
win32AppIncludeDirs["Microsoft"] 				= engineThirdPartyDir .. "Microsoft/"
win32AppIncludeDirs["Nvidia"]					= engineThirdPartyDir .. "Nvidia/"
win32AppIncludeDirs["spdlog"]					= engineThirdPartyDir .. "spdlog/include/"
win32AppIncludeDirs["rapidjson"] 				= engineThirdPartyDir .. "rapidjson/include/"
win32AppIncludeDirs["Mono"]						= monoInstallDir .. "include/"
win32AppIncludeDirs["Engine_Source_Root"]		= rootDirPath .. "Engine_Source/"
win32AppIncludeDirs["Engine_Source_Src"]			= rootDirPath .. "Engine_Source/Source/"
win32AppIncludeDirs["Engine_Source_Third_Party"]	= rootDirPath .. "Engine_Source/Third_Party/"
win32AppIncludeDirs["Build_Rules"]				= rootDirPath .. "Build_Rules/"

project (projectName)
	location (rootDirPath .. projectName)
	kind ("WindowedApp")
	cppdialect ("C++17")
	language ("C++")
	staticruntime ("off")
	targetname (projectName)
	
	targetdir (rootDirPath .. "Binaries/" .. outputdir .. "/%{prj.name}")
    objdir (rootDirPath .. "Binaries/Intermediates/" .. outputdir .. "/%{prj.name}")

	files
	{
		"Application-Win32-Make.lua",

		-- Personal Source Files for this Application
		"Source/**.h",
		"Source/**.cpp",
	}

	includedirs
	{
		"%{win32AppIncludeDirs.assimp}",
		"%{win32AppIncludeDirs.Microsoft}",
		"%{win32AppIncludeDirs.Nvidia}DirectX12/",
		"%{win32AppIncludeDirs.Microsoft}DirectX12/WinPixEventRuntime.1.0.161208001/Include/",
		"%{win32AppIncludeDirs.spdlog}",
		"%{win32AppIncludeDirs.rapidjson}",
		"%{win32AppIncludeDirs.Mono}mono-2.0/",
		"%{win32AppIncludeDirs.Engine_Source_Src}/",
		"%{win32AppIncludeDirs.Engine_Source_Third_Party}/",
		
		-- Personal Source Files for this Application
		"Source/",

		-- Shared Header Includes for this Project
		"%{win32AppIncludeDirs.Build_Rules}/PCH_Source/",
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
		"WinPixEventRuntime_UAP.lib",
		"Shlwapi.lib",
		"DirectXTK.lib",
        "DirectXTK12.lib",
		
		-- Set the Runtime Library to Win32
		"Engine_Build_Win32",
	}

	systemversion ("latest")
	defines
	{
		"IE_PLATFORM_BUILD_WIN32",
	}
	flags
	{
		"MultiProcessorCompile"
	}
	postbuildcommands
	{
		-- Assimp
		("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/assimp-5.0.1/build/code/Debug/assimp-vc142-mtd.dll ../Binaries/" .. outputdir .. "/" .. projectName),
		-- DX11 Debug Layers
		("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/Bin/D3D11SDKLayers.dll ../Binaries/" .. outputdir .. "/" .. projectName),
		("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/Bin/D3DX11d_43.dll ../Binaries/"..outputdir.."/" .. projectName),
		("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/Bin/D3D11Ref.dll ../Binaries/"..outputdir.."/" .. projectName),
		-- Mono
		("{COPY} \"".. monoInstallDir .."/bin/mono-2.0-sgen.dll\" ../Binaries/" .. outputdir .. "/" .. projectName),
		-- PIX
		("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/WinPixEventRuntime.dll ../Binaries/"..outputdir.."/" .. projectName),
		("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/WinPixEventRuntime_UAP.dll ../Binaries/"..outputdir.."/" .. projectName),
		-- Copy over assets
		("{COPY} %{wks.location}Content ../Binaries/" .. outputdir .. "/Content"),
		-- Copy over default engine assets
		("{COPY} ../Engine_Source/Assets ../Binaries/"..outputdir.."/Content/Engine")
	}



-- Build Configurations

	filter "configurations:Debug"
		defines "IE_DEBUG"
		symbols "on"
		libdirs
		{
            "%{win32AppIncludeDirs.Engine_Source_Third_Party}/assimp-5.0.1/build/code/Debug/",
            "%{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/",
            "%{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Debug/",
            "%{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Debug/",
			monoInstallDir .. "/lib/",
		}
	
	filter "configurations:Release"
		defines "IE_RELEASE"
		symbols "on"
		optimize "on"
		libdirs
		{
			"%{win32AppIncludeDirs.Engine_Source_Third_Party}/assimp-5.0.1/build/code/Release",
            "%{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/",
            "%{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release",
			"%{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/DXTex/DirectXTex/Bin/Desktop_2019_Win10/x64/Release",
            "%{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release",
			monoInstallDir .. "/lib",
		}
		postbuildcommands
		{
			-- Assimp
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/assimp-5.0.1/build/code/Release/assimp-vc140-mt.dll ../Binaries/"..outputdir.."/Engine"),
			-- DX11 Debug Layers	
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/Bin/D3D11SDKLayers.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/Bin/D3DX11d_43.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/Bin/D3D11Ref.dll ../Binaries/"..outputdir.."/Engine"),
			-- Mono					
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Mono/bin/mono-2.0-sgen.dll ../Binaries/"..outputdir.."/Engine"),
			-- DirectX				
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/Bin/dxcompiler.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/Bin/dxil.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{wks.location}/Engine/Source/Shaders/HLSL/Ray_Tracing/** ../Binaries/" .. outputdir.."/Engine"),
			-- PIX					
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/WinPixEventRuntime.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/WinPixEventRuntime_UAP.dll ../Binaries/"..outputdir.."/Engine"),
			-- Copy over assets
			("{COPY} $(USERPROFILE)/Documents/Insight-Projects/Development-Project/Content/** ../Binaries/" .. outputdir .. "/Content"),
			("{COPY} %{wks.location}/Engine_Source/Assets/Textures/Default_Object/** ../Binaries/"..outputdir.."/Content/Default_Assets/")
		}

	filter "configurations:EngineDist"
		defines "IE_ENGINE_DIST"
		optimize "on"
		symbols "on"
		libdirs
		{
			"%{win32AppIncludeDirs.Engine_Source_Third_Party}/assimp-5.0.1/build/code/Release",
            "%{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/",
            "%{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release",
			"%{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/DXTex/DirectXTex/Bin/Desktop_2019_Win10/x64/Release",
            "%{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release",
			monoInstallDir .. "/lib",
		}
		postbuildcommands
		{
			-- assimp
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/assimp-5.0.1/build/code/Release/assimp-vc140-mt.dll ../Binaries/"..outputdir.."/Engine"),
			-- mono
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Mono/bin/mono-2.0-sgen.dll ../Binaries/"..outputdir.."/Engine"),
			-- DirectX
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/Bin/dxcompiler.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/Bin/dxil.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{wks.location}/Engine/Source/Shaders/HLSL/Ray_Tracing/** ../Binaries/" .. outputdir.."/Engine"),
			-- Copy over assets
			("{COPY} %{wks.location}/Engine_Source/Assets/Textures/Default_Object/** ../Binaries/"..outputdir.."/Content/Default_Assets/")
		}
		
	filter "configurations:GameDist"
		defines "IE_GAME_DIST"
		optimize "on"
		symbols "on"
		libdirs
		{
            "%{win32AppIncludeDirs.Engine_Source_Third_Party}/assimp-5.0.1/build/code/Release",            
            "%{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/",
            "%{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release",
			"%{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/DXTex/DirectXTex/Bin/Desktop_2019_Win10/x64/Release",
            "%{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release",
			monoInstallDir .. "/lib",
		}
		postbuildcommands
		{
			-- assimp
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/assimp-5.0.1/build/code/Release/assimp-vc140-mt.dll ../Binaries/"..outputdir.."/Engine"),
			-- mono
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Mono/bin/mono-2.0-sgen.dll ../Binaries/"..outputdir.."/Engine"),
			-- DirectX
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/Bin/dxcompiler.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/Bin/dxil.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{wks.location}/Engine/Source/Shaders/HLSL/Ray_Tracing/** ../Binaries/" .. outputdir.."/Engine"),
			("{COPY} %{wks.location}/Engine_Source/Assets/Textures/Default_Object/** ../Binaries/"..outputdir.."/Content/Default_Assets/")
		}

	