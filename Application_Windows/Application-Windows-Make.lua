-- Application

appName = "Application"

engineThirdPartyDir = "../Engine/Third_Party/"
monoInstallDir = "C:/Program Files/Mono/"
rootDirPath = "../"

applicationIncludeDirs = {}
applicationIncludeDirs["assimp"]	= engineThirdPartyDir .. "assimp-3.3.1/include"
applicationIncludeDirs["Microsoft"] = engineThirdPartyDir .. "Microsoft"
applicationIncludeDirs["Nvidia"]	= engineThirdPartyDir .. "Nvidia"
applicationIncludeDirs["spdlog"]	= engineThirdPartyDir .. "spdlog/include"
applicationIncludeDirs["rapidjson"] = engineThirdPartyDir .. "rapidjson/include"
applicationIncludeDirs["Mono"]		= monoInstallDir .. "include"
applicationIncludeDirs["Engine"]	= rootDirPath .. "Engine"

project (appName .. "_Windows")
	location (rootDirPath .. appName .. "_Windows")
	kind "WindowedApp"
	cppdialect "C++17"
	language "C++"
	staticruntime "off"
	targetname(appName .. "_Windows")
	
	targetdir (rootDirPath .. "Binaries/" .. outputdir .. "/%{prj.name}")
    objdir (rootDirPath .. "Binaries/Intermediates/" .. outputdir .. "/%{prj.name}")

	files
	{
		"Application-Windows-Make.lua",
		"Source/**.h",
		"Source/**.cpp",
	}

	includedirs
	{
		"%{applicationIncludeDirs.assimp}/",
		"%{applicationIncludeDirs.Microsoft}/",
		"%{applicationIncludeDirs.Nvidia}/DirectX12/",
		"%{applicationIncludeDirs.Microsoft}/DirectX12/WinPixEventRuntime.1.0.161208001/Include/",
		"%{applicationIncludeDirs.spdlog}/",
		"%{applicationIncludeDirs.rapidjson}/",
		"%{applicationIncludeDirs.Mono}/mono-2.0/",
		"%{applicationIncludeDirs.Engine}/Source/",
		"%{applicationIncludeDirs.Engine}/Third_Party/",

		"Source/"
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
		
		"Engine",
	}

	filter "system:windows"
		systemversion "latest"
		defines
		{
			"IE_PLATFORM_WINDOWS"
		}
		flags
		{
			"MultiProcessorCompile"
		}
		postbuildcommands
		{
			-- Assimp
			("{COPY} " .. engineThirdPartyDir .. "/assimp-3.3.1/build/code/Debug/assimp-vc140-mt.dll ../Binaries/" .. outputdir .. "/" .. appName .. "_Windows"),
			-- DX11 Debug Layers
			("{COPY} " .. engineThirdPartyDir .. "/Microsoft/DirectX11/Bin/D3D11SDKLayers.dll ../Binaries/" .. outputdir .. "/" .. appName .. "_Windows"),
			("{COPY} " .. engineThirdPartyDir .. "/Microsoft/DirectX11/Bin/D3DX11d_43.dll ../Binaries/"..outputdir.."/" .. appName .. "_Windows"),
			("{COPY} " .. engineThirdPartyDir .. "/Microsoft/DirectX11/Bin/D3D11Ref.dll ../Binaries/"..outputdir.."/" .. appName .. "_Windows"),
			-- Mono
			("{COPY} \"".. monoInstallDir .."/bin/mono-2.0-sgen.dll\" ../Binaries/" .. outputdir .. "/" .. appName .. "_Windows"),
			-- DirectX
			("{COPY} ".. engineThirdPartyDir .."/Microsoft/DirectX12/Bin/dxcompiler.dll ../Binaries/"..outputdir.."/" .. appName .. "_Windows"),
			("{COPY} ".. engineThirdPartyDir .."/Microsoft/DirectX12/Bin/dxil.dll ../Binaries/"..outputdir.."/" .. appName .. "_Windows"),
			("{COPY} ../Engine/Source/Shaders/HLSL/Ray_Tracing/** ../Binaries/" .. outputdir.."/Engine"),
			-- PIX
			("{COPY} ".. engineThirdPartyDir .."/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/WinPixEventRuntime.dll ../Binaries/"..outputdir.."/" .. appName .. "_Windows"),
			("{COPY} ".. engineThirdPartyDir .."/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/WinPixEventRuntime_UAP.dll ../Binaries/"..outputdir.."/" .. appName .. "_Windows"),
			-- Copy over default engine assets
			("{COPY} ../Engine/Assets/Textures/Default_Object/** ../Binaries/"..outputdir.."/Default_Assets/")
		}



-- Build Configurations

	filter "configurations:Debug"
		defines "IE_DEBUG"
		symbols "on"
		defines
        {
            "IE_DEBUG"
        }
		libdirs
		{
            engineThirdPartyDir .. "/assimp-3.3.1/build/code/Debug/",
            engineThirdPartyDir .. "/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/",
            engineThirdPartyDir .. "/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Debug/",
            engineThirdPartyDir .. "/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Debug/",
			monoInstallDir .. "/lib/",
		}
	
	filter "configurations:Release"
		defines "IE_RELEASE"
		symbols "on"
		optimize "on"
		libdirs
		{
			engineThirdPartyDir .. "/assimp-3.3.1/build/code/Release",
            engineThirdPartyDir .. "/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/",
            engineThirdPartyDir .. "/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release",
			engineThirdPartyDir .. "/Microsoft/DirectX12/DXTex/DirectXTex/Bin/Desktop_2019_Win10/x64/Release",
            engineThirdPartyDir .. "/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release",
			monoInstallDir .. "/lib",
		}
		postbuildcommands
		{
			-- Assimp
			("{COPY} ".. engineThirdPartyDir .."/assimp-3.3.1/build/code/Release/assimp-vc140-mt.dll ../Binaries/"..outputdir.."/Engine"),
			-- DX11 Debug Layers	
			("{COPY} ".. engineThirdPartyDir .."/Microsoft/DirectX11/Bin/D3D11SDKLayers.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} ".. engineThirdPartyDir .."/Microsoft/DirectX11/Bin/D3DX11d_43.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} ".. engineThirdPartyDir .."/Microsoft/DirectX11/Bin/D3D11Ref.dll ../Binaries/"..outputdir.."/Engine"),
			-- Mono					
			("{COPY} ".. engineThirdPartyDir .."/Mono/bin/mono-2.0-sgen.dll ../Binaries/"..outputdir.."/Engine"),
			-- DirectX				
			("{COPY} ".. engineThirdPartyDir .."/Microsoft/DirectX12/Bin/dxcompiler.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} ".. engineThirdPartyDir .."/Microsoft/DirectX12/Bin/dxil.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{wks.location}/Engine/Source/Shaders/HLSL/Ray_Tracing/** ../Binaries/" .. outputdir.."/Engine"),
			-- PIX					
			("{COPY} ".. engineThirdPartyDir .."/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/WinPixEventRuntime.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} ".. engineThirdPartyDir .."/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/WinPixEventRuntime_UAP.dll ../Binaries/"..outputdir.."/Engine"),
			-- Copy over default engine assets
			("{COPY} %{wks.location}/Engine_Source/Assets/Textures/Default_Object/** ../Binaries/"..outputdir.."/Default_Assets/")
		}

	filter "configurations:EngineDist"
		defines "IE_ENGINE_DIST"
		optimize "on"
		symbols "on"
		libdirs
		{
			engineThirdPartyDir .. "/assimp-3.3.1/build/code/Release",
            engineThirdPartyDir .. "/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/",
            engineThirdPartyDir .. "/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release",
			engineThirdPartyDir .. "/Microsoft/DirectX12/DXTex/DirectXTex/Bin/Desktop_2019_Win10/x64/Release",
            engineThirdPartyDir .. "/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release",
			monoInstallDir .. "/lib",
		}
		postbuildcommands
		{
			-- assimp
			("{COPY} ".. engineThirdPartyDir .."/assimp-3.3.1/build/code/Release/assimp-vc140-mt.dll ../Binaries/"..outputdir.."/Engine"),
			-- mono
			("{COPY} ".. engineThirdPartyDir .."/Mono/bin/mono-2.0-sgen.dll ../Binaries/"..outputdir.."/Engine"),
			-- DirectX
			("{COPY} ".. engineThirdPartyDir .."/Microsoft/DirectX12/Bin/dxcompiler.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} ".. engineThirdPartyDir .."/Microsoft/DirectX12/Bin/dxil.dll ../Binaries/"..outputdir.."/Engine"),
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
            engineThirdPartyDir .. "/assimp-3.3.1/build/code/Release",            
            engineThirdPartyDir .. "/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/",
            engineThirdPartyDir .. "/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release",
			engineThirdPartyDir .. "/Microsoft/DirectX12/DXTex/DirectXTex/Bin/Desktop_2019_Win10/x64/Release",
            engineThirdPartyDir .. "/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release",
			monoInstallDir .. "/lib",
		}
		postbuildcommands
		{
			-- assimp
			("{COPY} ".. engineThirdPartyDir .."/assimp-3.3.1/build/code/Release/assimp-vc140-mt.dll ../Binaries/"..outputdir.."/Engine"),
			-- mono
			("{COPY} ".. engineThirdPartyDir .."/Mono/bin/mono-2.0-sgen.dll ../Binaries/"..outputdir.."/Engine"),
			-- DirectX
			("{COPY} ".. engineThirdPartyDir .."/Microsoft/DirectX12/Bin/dxcompiler.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} ".. engineThirdPartyDir .."/Microsoft/DirectX12/Bin/dxil.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{wks.location}/Engine/Source/Shaders/HLSL/Ray_Tracing/** ../Binaries/" .. outputdir.."/Engine"),
		}

	