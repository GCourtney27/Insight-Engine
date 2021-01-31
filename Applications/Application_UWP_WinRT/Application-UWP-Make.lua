-- UWP Application


appName = "Application"
projectName = appName .. "_UWP_WinRT"

rootDirPath = "../../"
engineThirdPartyDir = rootDirPath .. "Engine/Third_Party/"

uwpAppIncludeDirs = {}
uwpAppIncludeDirs["tinyobjloader"]			= engineThirdPartyDir .. "tinyobjloader/include/"
uwpAppIncludeDirs["assimp"]					= engineThirdPartyDir .. "assimp-5.0.1/include/"
uwpAppIncludeDirs["OpenFBX"]				= engineThirdPartyDir .. "OpenFBX/src/"
uwpAppIncludeDirs["Microsoft"] 				= engineThirdPartyDir .. "Microsoft/"
uwpAppIncludeDirs["Nvidia"]					= engineThirdPartyDir .. "Nvidia/"
uwpAppIncludeDirs["spdlog"]					= engineThirdPartyDir .. "spdlog/include/"
uwpAppIncludeDirs["rapidjson"] 				= engineThirdPartyDir .. "rapidjson/include/"
uwpAppIncludeDirs["Engine_Root"]			= rootDirPath .. "Engine/"
uwpAppIncludeDirs["Engine_Src"]				= rootDirPath .. "Engine/Source/"
uwpAppIncludeDirs["Engine_Third_Party"]		= rootDirPath .. "Engine/Third_Party/"
uwpAppIncludeDirs["Build_Rules"]			= rootDirPath .. "Build_Rules/"
uwpAppIncludeDirs["Game_Runtime"]			= rootDirPath .. "Game_Runtime/Source/"

project (projectName)
	location (rootDirPath.. "Applications/" .. projectName)
	kind ("WindowedApp")
	language ("C++")
	cppdialect ("C++17")
	staticruntime ("off")
	systemversion ("latest")
	targetname (projectName)
	
	targetdir (rootDirPath .. binaryDirectory .. "%{prj.name}")
	objdir (rootDirPath .. intDirectory .. "%{prj.name}")

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

		-- Deploy Assets
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
		"%{uwpAppIncludeDirs.spdlog}",
		"%{uwpAppIncludeDirs.rapidjson}",
		"%{uwpAppIncludeDirs.Engine_Src}/",
		"%{uwpAppIncludeDirs.Engine_Third_Party}/",
		
		-- Personal Source Files for this Application
		"Source/",

		-- Shared Header Includes for this Project
		"%{uwpAppIncludeDirs.Build_Rules}/PCH_Source/",

	}

	links
	{     
		-- Third Party   
		--"assimp-vc142-mtd.lib",

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

	defines
	{
		-- Compile for UWP platform
		"IE_PLATFORM_BUILD_UWP",
	}
	flags
	{
		"MultiProcessorCompile"
	}
	postbuildcommands
	{
		--("{COPY} %{uwpAppIncludeDirs.Engine_Third_Party}/assimp-5.0.1/build/code/Release/assimp-vc140-mt.dll ../Binaries/"..outputdir.."/Engine"),
		-- DX11 Debug Layers
		("{COPY} %{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/DirectX11/Bin/D3D11SDKLayers.dll %{cfg.targetdir}"),
		("{COPY} %{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/DirectX11/Bin/D3DX11d_43.dll %{cfg.targetdir}"),
		("{COPY} %{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/DirectX11/Bin/D3D11Ref.dll %{cfg.targetdir}"),
		-- DirectX
		("{COPY} %{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/DirectX12/Bin/dxcompiler.dll %{cfg.targetdir}"),
		("{COPY} %{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/DirectX12/Bin/dxil.dll %{cfg.targetdir}"),
		-- PIX
		("{COPY} %{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/WinPixEventRuntime/bin/x64/WinPixEventRuntime_UAP.dll %{cfg.targetdir}"),
		-- Copy over assets
		("{COPY} %{wks.location}Content %{cfg.targetdir}/../Content"),
		-- Copy over default engine assets
		("{COPY} ../../Engine/Assets %{cfg.targetdir}/../Content/Engine"),
	}

		
	-- Build Configurations

	filter "configurations:Debug*"
	defines "IE_DEBUG"
	symbols "on"
	libdirs
	{
		"%{uwpAppIncludeDirs.Engine_Third_Party}/assimp-5.0.1/build/code/Debug/",
		"%{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/WinPixEventRuntime/bin/x64",
		"%{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Debug/",
		"%{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Debug/",
		monoInstallDir .. "/lib/",
	}

	filter "configurations:Release*"
		defines "IE_RELEASE"
		symbols "on"
		optimize "on"
		libdirs
		{
			"%{uwpAppIncludeDirs.Engine_Third_Party}/assimp-3.3.1/build/code/Release",
			"%{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/WinPixEventRuntime/bin/x64",
			"%{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release",
			"%{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release",
			monoInstallDir .. "/lib",
		}
		postbuildcommands
		{
			-- Assimp
			("{COPY} %{uwpAppIncludeDirs.Engine_Third_Party}/assimp-3.3.1/build/code/Release/assimp-vc140-mt.dll ../Binaries/"..outputdir.."/Engine"),
			-- DX11 Debug Layers	
			("{COPY} %{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/DirectX11/Bin/D3D11SDKLayers.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/DirectX11/Bin/D3DX11d_43.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/DirectX11/Bin/D3D11Ref.dll ../Binaries/"..outputdir.."/Engine"),
			-- Mono					
			("{COPY} %{uwpAppIncludeDirs.Engine_Third_Party}/Mono/bin/mono-2.0-sgen.dll ../Binaries/"..outputdir.."/Engine"),
			-- DirectX				
			("{COPY} %{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/DirectX12/Bin/dxcompiler.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/DirectX12/Bin/dxil.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{wks.location}/Engine/Source/Shaders/HLSL/Ray_Tracing/** ../Binaries/" .. outputdir.."/Engine"),
			-- PIX					
			("{COPY} %{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/WinPixEventRuntime/bin/x64/WinPixEventRuntime_UAP.dll ../Binaries/"..outputdir.."/Engine"),
			-- Copy over assets
			("{COPY} $(USERPROFILE)/Documents/Insight-Projects/Development-Project/Content/** ../Binaries/" .. outputdir .. "/Content"),
			("{COPY} %{wks.location}/Engine/Assets/Textures/Default_Object/** ../Binaries/"..outputdir.."/Content/Default_Assets/")
		}

	filter "configurations:EngineDist"
		defines "IE_ENGINE_DIST"
		optimize "on"
		symbols "on"
		libdirs
		{
			"%{uwpAppIncludeDirs.Engine_Third_Party}/assimp-3.3.1/build/code/Release",
			"%{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/WinPixEventRuntime/bin/x64",
			"%{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release",
			"%{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release",
			monoInstallDir .. "/lib",
		}
		postbuildcommands
		{
			-- assimp
			("{COPY} %{uwpAppIncludeDirs.Engine_Third_Party}/assimp-3.3.1/build/code/Release/assimp-vc140-mt.dll ../Binaries/"..outputdir.."/Engine"),
			-- mono
			("{COPY} %{uwpAppIncludeDirs.Engine_Third_Party}/Mono/bin/mono-2.0-sgen.dll ../Binaries/"..outputdir.."/Engine"),
			-- DirectX
			("{COPY} %{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/DirectX12/Bin/dxcompiler.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/DirectX12/Bin/dxil.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{wks.location}/Engine/Source/Shaders/HLSL/Ray_Tracing/** ../Binaries/" .. outputdir.."/Engine"),
			-- Copy over assets
			("{COPY} %{wks.location}/Engine/Assets/Textures/Default_Object/** ../Binaries/"..outputdir.."/Content/Default_Assets/")
		}

	filter "configurations:Dist*"
		defines "IE_GAME_DIST"
		optimize "on"
		symbols "on"
		libdirs
		{
			"%{uwpAppIncludeDirs.Engine_Third_Party}/assimp-3.3.1/build/code/Release",            
			"%{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/WinPixEventRuntime/bin/x64",
			"%{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release",
			"%{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/DirectX12/DXTex/DirectXTex/Bin/Desktop_2019_Win10/x64/Release",
			"%{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release",
			monoInstallDir .. "/lib",
		}
		postbuildcommands
		{
			-- assimp
			("{COPY} %{uwpAppIncludeDirs.Engine_Third_Party}/assimp-3.3.1/build/code/Release/assimp-vc140-mt.dll ../Binaries/"..outputdir.."/Engine"),
			-- mono
			("{COPY} %{uwpAppIncludeDirs.Engine_Third_Party}/Mono/bin/mono-2.0-sgen.dll ../Binaries/"..outputdir.."/Engine"),
			-- DirectX
			("{COPY} %{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/DirectX12/Bin/dxcompiler.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{uwpAppIncludeDirs.Engine_Third_Party}/Microsoft/DirectX12/Bin/dxil.dll ../Binaries/"..outputdir.."/Engine"),
			("{COPY} %{wks.location}/Engine/Source/Shaders/HLSL/Ray_Tracing/** ../Binaries/" .. outputdir.."/Engine"),
			("{COPY} %{wks.location}/Engine/Assets/Textures/Default_Object/** ../Binaries/"..outputdir.."/Content/Default_Assets/")
		}
