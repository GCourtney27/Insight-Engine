-- Application
-- The client exe that gets executed and runs.

appName = "Application"
projectName = appName .. "_Win32"

engineThirdPartyDir = "../Engine_Source/Third_Party/"
monoInstallDir = "C:/Program Files/Mono/"
rootDirPath = "../"

applicationIncludeDirs = {}
applicationIncludeDirs["assimp"]					= engineThirdPartyDir .. "assimp-3.3.1/include/"
applicationIncludeDirs["Microsoft"] 				= engineThirdPartyDir .. "Microsoft/"
applicationIncludeDirs["Nvidia"]					= engineThirdPartyDir .. "Nvidia/"
applicationIncludeDirs["spdlog"]					= engineThirdPartyDir .. "spdlog/include/"
applicationIncludeDirs["rapidjson"] 				= engineThirdPartyDir .. "rapidjson/include/"
applicationIncludeDirs["Mono"]						= monoInstallDir .. "include/"
applicationIncludeDirs["Engine_Source_Root"]		= rootDirPath .. "Engine_Source/"
applicationIncludeDirs["Engine_Source_Src"]			= rootDirPath .. "Engine_Source/Source/"
applicationIncludeDirs["Engine_Source_Third_Party"]	= rootDirPath .. "Engine_Source/Third_Party/"
applicationIncludeDirs["Build_Rules"]				= rootDirPath .. "Build_Rules/"

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
		engineThirdPartyDir .. "assimp-3.3.1/include/",
		-- "%{applicationIncludeDirs.assimp}",
		"%{applicationIncludeDirs.Microsoft}",
		"%{applicationIncludeDirs.Nvidia}DirectX12/",
		"%{applicationIncludeDirs.Microsoft}DirectX12/WinPixEventRuntime.1.0.161208001/Include/",
		"%{applicationIncludeDirs.spdlog}",
		"%{applicationIncludeDirs.rapidjson}",
		"%{applicationIncludeDirs.Mono}/mono-2.0/",
		"%{applicationIncludeDirs.Engine_Source_Src}/",
		"%{applicationIncludeDirs.Engine_Source_Third_Party}/",
		
		-- Personal Source Files for this Application
		"Source/",

		-- Shared Header Includes for this Project
		"%{applicationIncludeDirs.Build_Rules}/PCH_Source/",
	}

	links
	{
        -- Third Party
		"MonoPosixHelper.lib",
		"mono-2.0-sgen.lib",
        "libmono-static-sgen.lib",
		"assimp-vc140-mt.lib",
        
        -- DirectX/Windows API
		"d3d12.lib",
		"dxgi.lib",
		"d3d11.lib",
		"WinPixEventRuntime.lib",
		"WinPixEventRuntime_UAP.lib",
		"Shlwapi.lib",
		"DirectXTK.lib",
		"D3Dcompiler.lib",
		"dxcompiler.lib",
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
		("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/assimp-3.3.1/build/code/Debug/assimp-vc140-mt.dll ../Binaries/" .. outputdir .. "/" .. projectName),
		-- DX11 Debug Layers
		("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/Bin/D3D11SDKLayers.dll ../Binaries/" .. outputdir .. "/" .. projectName),
		("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/Bin/D3DX11d_43.dll ../Binaries/"..outputdir.."/" .. projectName),
		("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/Bin/D3D11Ref.dll ../Binaries/"..outputdir.."/" .. projectName),
		-- Mono
		("{COPY} \"".. monoInstallDir .."/bin/mono-2.0-sgen.dll\" ../Binaries/" .. outputdir .. "/" .. projectName),
		-- DirectX
		("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/Bin/dxcompiler.dll ../Binaries/"..outputdir.."/" .. projectName),
		("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/Bin/dxil.dll ../Binaries/"..outputdir.."/" .. projectName),
		-- PIX
		("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/WinPixEventRuntime.dll ../Binaries/"..outputdir.."/" .. projectName),
		("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/WinPixEventRuntime_UAP.dll ../Binaries/"..outputdir.."/" .. projectName),
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
            "%{applicationIncludeDirs.Engine_Source_Third_Party}/assimp-3.3.1/build/code/Debug/",
            "%{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/",
            "%{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Debug/",
            "%{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Debug/",
			monoInstallDir .. "/lib/",
		}
	
	filter "configurations:Release"
		defines "IE_RELEASE"
		symbols "on"
		optimize "on"
		libdirs
		{
			"%{applicationIncludeDirs.Engine_Source_Third_Party}/assimp-3.3.1/build/code/Release",
            "%{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/",
            "%{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release",
			"%{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/DXTex/DirectXTex/Bin/Desktop_2019_Win10/x64/Release",
            "%{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release",
			monoInstallDir .. "/lib",
		}
		postbuildcommands
		{
			-- Assimp
			("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/assimp-3.3.1/build/code/Release/assimp-vc140-mt.dll ../Binaries/"..outputdir.."/Engine"),
			-- DX11 Debug Layers	
			("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/Bin/D3D11SDKLayers.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/Bin/D3DX11d_43.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/Bin/D3D11Ref.dll ../Binaries/"..outputdir.."/Engine"),
			-- Mono					
			("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Mono/bin/mono-2.0-sgen.dll ../Binaries/"..outputdir.."/Engine"),
			-- DirectX				
			("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/Bin/dxcompiler.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/Bin/dxil.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{wks.location}/Engine/Source/Shaders/HLSL/Ray_Tracing/** ../Binaries/" .. outputdir.."/Engine"),
			-- PIX					
			("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/WinPixEventRuntime.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/WinPixEventRuntime_UAP.dll ../Binaries/"..outputdir.."/Engine"),
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
			"%{applicationIncludeDirs.Engine_Source_Third_Party}/assimp-3.3.1/build/code/Release",
            "%{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/",
            "%{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release",
			"%{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/DXTex/DirectXTex/Bin/Desktop_2019_Win10/x64/Release",
            "%{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release",
			monoInstallDir .. "/lib",
		}
		postbuildcommands
		{
			-- assimp
			("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/assimp-3.3.1/build/code/Release/assimp-vc140-mt.dll ../Binaries/"..outputdir.."/Engine"),
			-- mono
			("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Mono/bin/mono-2.0-sgen.dll ../Binaries/"..outputdir.."/Engine"),
			-- DirectX
			("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/Bin/dxcompiler.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/Bin/dxil.dll ../Binaries/"..outputdir.."/Engine"),
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
            "%{applicationIncludeDirs.Engine_Source_Third_Party}/assimp-3.3.1/build/code/Release",            
            "%{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/",
            "%{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release",
			"%{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/DXTex/DirectXTex/Bin/Desktop_2019_Win10/x64/Release",
            "%{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release",
			monoInstallDir .. "/lib",
		}
		postbuildcommands
		{
			-- assimp
			("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/assimp-3.3.1/build/code/Release/assimp-vc140-mt.dll ../Binaries/"..outputdir.."/Engine"),
			-- mono
			("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Mono/bin/mono-2.0-sgen.dll ../Binaries/"..outputdir.."/Engine"),
			-- DirectX
			("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/Bin/dxcompiler.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/Bin/dxil.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{wks.location}/Engine/Source/Shaders/HLSL/Ray_Tracing/** ../Binaries/" .. outputdir.."/Engine"),
			("{COPY} %{wks.location}/Engine_Source/Assets/Textures/Default_Object/** ../Binaries/"..outputdir.."/Content/Default_Assets/")
		}

	