-- Application

appName = "Application"

engineVendorDir = "../Engine/Vendor/"
rootDirPath = "../"

applicationIncludeDirs = {}
applicationIncludeDirs["assimp"]	= engineVendorDir .. "assimp-3.3.1/include"
applicationIncludeDirs["Microsoft"] = engineVendorDir .. "Microsoft"
applicationIncludeDirs["Nvidia"]	= engineVendorDir .. "Nvidia"
applicationIncludeDirs["spdlog"]	= engineVendorDir .. "spdlog/include"
applicationIncludeDirs["rapidjson"] = engineVendorDir .. "rapidjson/include"
applicationIncludeDirs["Mono"]		= engineVendorDir .. "Mono/include"
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
		"%{applicationIncludeDirs.Engine}/Vendor/",

		"Source/"
	}

	links
	{
        -- Vendor
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
		"DirectXTex.lib",
		
		"Engine_Source",
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
			("{COPY} " .. engineVendorDir .. "/assimp-3.3.1/build/code/Debug/assimp-vc140-mt.dll ../Binaries/" .. outputdir .. "/" .. appName .. "_Windows"),
			-- DX11 Debug Layers
			("{COPY} " .. engineVendorDir .. "/Microsoft/DirectX11/Bin/D3D11SDKLayers.dll ../Binaries/" .. outputdir .. "/" .. appName .. "_Windows"),
			("{COPY} " .. engineVendorDir .. "/Microsoft/DirectX11/Bin/D3DX11d_43.dll ../Binaries/"..outputdir.."/" .. appName .. "_Windows"),
			("{COPY} " .. engineVendorDir .. "/Microsoft/DirectX11/Bin/D3D11Ref.dll ../Binaries/"..outputdir.."/" .. appName .. "_Windows"),
			-- Mono
			("{COPY} ".. engineVendorDir .."/Mono/bin/mono-2.0-sgen.dll ../Binaries/"..outputdir.."/" .. appName .. "_Windows"),
			-- DirectX
			("{COPY} ".. engineVendorDir .."/Microsoft/DirectX12/Bin/dxcompiler.dll ../Binaries/"..outputdir.."/" .. appName .. "_Windows"),
			("{COPY} ".. engineVendorDir .."/Microsoft/DirectX12/Bin/dxil.dll ../Binaries/"..outputdir.."/" .. appName .. "_Windows"),
			("{COPY} ../Engine/Source/Shaders/HLSL/Ray_Tracing/** ../Binaries/" .. outputdir.."/Engine"),
			-- PIX
			("{COPY} ".. engineVendorDir .."/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/WinPixEventRuntime.dll ../Binaries/"..outputdir.."/" .. appName .. "_Windows"),
			("{COPY} ".. engineVendorDir .."/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/WinPixEventRuntime_UAP.dll ../Binaries/"..outputdir.."/" .. appName .. "_Windows"),
			-- Copy over default engine assets
			("{COPY} ../Engine/Assets/Textures/Default_Object/** ../Binaries/"..outputdir.."/Default_Assets/")
		}
	filter "configurations:Debug"
		defines "IE_DEBUG"
		symbols "on"
		defines
        {
            "IE_DEBUG"
        }
		
		libdirs
		{
            engineVendorDir .. "/assimp-3.3.1/build/code/Debug",
            engineVendorDir .. "/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/",
            engineVendorDir .. "/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Debug",
            engineVendorDir .. "/Microsoft/DirectX12/DXTex/DirectXTex/Bin/Desktop_2019_Win10/x64/Debug",
            engineVendorDir .. "/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Debug",
			engineVendorDir .. "/Mono/lib",
		}
	
	filter "configurations:Release"
		defines "IE_RELEASE"
		symbols "on"
		optimize "on"
		libdirs
		{
			"Vendor/assimp-3.3.1/build/code/Release",
            "Vendor/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/",
            "Vendor/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release",
			"Vendor/Microsoft/DirectX12/DXTex/DirectXTex/Bin/Desktop_2019_Win10/x64/Release",
            "Vendor/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release",
			"Vendor/Mono/lib",
		}
		postbuildcommands
		{
			-- Assimp
			("{COPY} ".. engineVendorDir .."/assimp-3.3.1/build/code/Release/assimp-vc140-mt.dll ../Binaries/"..outputdir.."/Engine"),
			-- DX11 Debug Layers	
			("{COPY} ".. engineVendorDir .."/Microsoft/DirectX11/Bin/D3D11SDKLayers.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} ".. engineVendorDir .."/Microsoft/DirectX11/Bin/D3DX11d_43.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} ".. engineVendorDir .."/Microsoft/DirectX11/Bin/D3D11Ref.dll ../Binaries/"..outputdir.."/Engine"),
			-- Mono					
			("{COPY} ".. engineVendorDir .."/Mono/bin/mono-2.0-sgen.dll ../Binaries/"..outputdir.."/Engine"),
			-- DirectX				
			("{COPY} ".. engineVendorDir .."/Microsoft/DirectX12/Bin/dxcompiler.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} ".. engineVendorDir .."/Microsoft/DirectX12/Bin/dxil.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{wks.location}/Engine/Source/Shaders/HLSL/Ray_Tracing/** ../Binaries/" .. outputdir.."/Engine"),
			-- PIX					
			("{COPY} ".. engineVendorDir .."/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/WinPixEventRuntime.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} ".. engineVendorDir .."/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/WinPixEventRuntime_UAP.dll ../Binaries/"..outputdir.."/Engine"),
			-- Copy over default engine assets
			("{COPY} %{wks.location}/Engine_Source/Assets/Textures/Default_Object/** ../Binaries/"..outputdir.."/Default_Assets/")
		}



	filter "configurations:EngineDist"
		defines "IE_ENGINE_DIST"
		optimize "on"
		symbols "on"
		libdirs
		{
			"Vendor/assimp-3.3.1/build/code/Release",
            "Vendor/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/",
            "Vendor/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release",
			"Vendor/Microsoft/DirectX12/DXTex/DirectXTex/Bin/Desktop_2019_Win10/x64/Release",
            "Vendor/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release",
			"Vendor/Mono/lib",
		}
		postbuildcommands
		{
			-- assimp
			("{COPY} ".. engineVendorDir .."/assimp-3.3.1/build/code/Release/assimp-vc140-mt.dll ../Binaries/"..outputdir.."/Engine"),
			-- mono
			("{COPY} ".. engineVendorDir .."/Mono/bin/mono-2.0-sgen.dll ../Binaries/"..outputdir.."/Engine"),
			-- DirectX
			("{COPY} ".. engineVendorDir .."/Microsoft/DirectX12/Bin/dxcompiler.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} ".. engineVendorDir .."/Microsoft/DirectX12/Bin/dxil.dll ../Binaries/"..outputdir.."/Engine"),
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
            "Vendor/assimp-3.3.1/build/code/Release",            
            "Vendor/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/",
            "Vendor/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release",
			"Vendor/Microsoft/DirectX12/DXTex/DirectXTex/Bin/Desktop_2019_Win10/x64/Release",
            "Vendor/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release",
			"Vendor/Mono/lib",
		}
		postbuildcommands
		{
			-- assimp
			("{COPY} ".. engineVendorDir .."/assimp-3.3.1/build/code/Release/assimp-vc140-mt.dll ../Binaries/"..outputdir.."/Engine"),
			-- mono
			("{COPY} ".. engineVendorDir .."/Mono/bin/mono-2.0-sgen.dll ../Binaries/"..outputdir.."/Engine"),
			-- DirectX
			("{COPY} ".. engineVendorDir .."/Microsoft/DirectX12/Bin/dxcompiler.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} ".. engineVendorDir .."/Microsoft/DirectX12/Bin/dxil.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{wks.location}/Engine/Source/Shaders/HLSL/Ray_Tracing/** ../Binaries/" .. outputdir.."/Engine"),
		}

	