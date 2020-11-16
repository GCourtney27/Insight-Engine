-- Application
-- The client exe that gets executed and runs.

appName = "Application"

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

project (appName .. "_Win32")
	location (rootDirPath .. appName .. "_Win32")
	kind "WindowedApp"
	cppdialect "C++17"
	language "C++"
	staticruntime "off"
	targetname(appName .. "_Win32")
	
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
		"%{applicationIncludeDirs.assimp}",
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

	systemversion "latest"
	defines
	{
		"IE_PLATFORM_BUILD_WIN32"
	}
	flags
	{
		"MultiProcessorCompile"
	}
	postbuildcommands
	{
		-- Assimp
		("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/assimp-3.3.1/build/code/Debug/assimp-vc140-mt.dll ../Binaries/" .. outputdir .. "/" .. appName .. "_Win32"),
		-- DX11 Debug Layers
		("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/Bin/D3D11SDKLayers.dll ../Binaries/" .. outputdir .. "/" .. appName .. "_Win32"),
		("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/Bin/D3DX11d_43.dll ../Binaries/"..outputdir.."/" .. appName .. "_Win32"),
		("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/Bin/D3D11Ref.dll ../Binaries/"..outputdir.."/" .. appName .. "_Win32"),
		-- Mono
		("{COPY} \"".. monoInstallDir .."/bin/mono-2.0-sgen.dll\" ../Binaries/" .. outputdir .. "/" .. appName .. "_Win32"),
		-- DirectX
		("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/Bin/dxcompiler.dll ../Binaries/"..outputdir.."/" .. appName .. "_Win32"),
		("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/Bin/dxil.dll ../Binaries/"..outputdir.."/" .. appName .. "_Win32"),
		("{COPY} ../Engine_Source/Source/Shaders/HLSL/Ray_Tracing/** ../Binaries/" .. outputdir.."/Engine_Build_Win32"),
		-- PIX
		("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/WinPixEventRuntime.dll ../Binaries/"..outputdir.."/" .. appName .. "_Win32"),
		("{COPY} %{applicationIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/WinPixEventRuntime_UAP.dll ../Binaries/"..outputdir.."/" .. appName .. "_Win32"),
		-- Copy over default engine assets
		("{COPY} ../Engine_Source/Assets/Textures/Default_Object/** ../Binaries/"..outputdir.."/Default_Assets/")
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
			-- Copy over default engine assets
			("{COPY} %{wks.location}/Engine_Source/Assets/Textures/Default_Object/** ../Binaries/"..outputdir.."/Default_Assets/")
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
			-- Copy over default engine assets
			("{COPY} %{wks.location}/Engine/Assets/Textures/Default_Object/** ../Binaries/"..outputdir.."/Default_Assets/")
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
		}

	