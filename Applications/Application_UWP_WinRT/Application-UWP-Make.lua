-- UWP Application

include ("../../Engine/BuildRules/Common-Build-Config.lua")


appName = "Application"
platform = "UWP"
projectName = appName .. "_UWP_WinRT"

rootDirPath = "../../"
engineThirdPartyDir = rootDirPath .. "Engine/ThirdParty/"

uwpAppIncludeDirs = {}
uwpAppIncludeDirs["tinyobjloader"]			= engineThirdPartyDir .. "tinyobjloader/include/"
uwpAppIncludeDirs["assimp"]					= engineThirdPartyDir .. "assimp-5.0.1/include/"
uwpAppIncludeDirs["OpenFBX"]				= engineThirdPartyDir .. "OpenFBX/src/"
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
		"IE_PLATFORM_BUILD_UWP=1",
	}
	flags
	{
		"MultiProcessorCompile"
	}
	postbuildcommands
	{
		--("{COPY} %{uwpAppIncludeDirs.Engine_ThirdParty}/assimp-5.0.1/build/code/Release/assimp-vc140-mt.dll ../Binaries/"..outputdir.."/Engine"),
		-- DX11 Debug Layers
		("{COPY} %{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX11/Bin/D3D11SDKLayers.dll %{cfg.targetdir}"),
		("{COPY} %{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX11/Bin/D3DX11d_43.dll %{cfg.targetdir}"),
		("{COPY} %{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX11/Bin/D3D11Ref.dll %{cfg.targetdir}"),
		-- DirectX
		("{COPY} %{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX12/Bin/dxcompiler.dll %{cfg.targetdir}"),
		("{COPY} %{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX12/Bin/dxil.dll %{cfg.targetdir}"),
		-- PIX
		("{COPY} %{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/WinPixEventRuntime/bin/x64/WinPixEventRuntime_UAP.dll %{cfg.targetdir}"),
		-- Copy over assets
		("{COPY} %{wks.location}Content %{cfg.targetdir}/Content"),
		-- Copy over default engine assets
		("{COPY} ../../Engine/Assets %{cfg.targetdir}/Content/Engine"),
	}

		
	-- Build Configurations

	filter "configurations:DebugEditor"
	libdirs
	{
		"%{uwpAppIncludeDirs.Engine_ThirdParty}/assimp-5.0.1/build/code/Debug/",
		"%{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/WinPixEventRuntime/bin/x64",
		"%{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Debug/",
		"%{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Debug/",
		monoInstallDir .. "/lib/",
	}

	filter "configurations:Development or DebugGame"
	libdirs
	{
		"%{uwpAppIncludeDirs.Engine_ThirdParty}/assimp-3.3.1/build/code/Release",
		"%{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/WinPixEventRuntime/bin/x64",
		"%{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release",
		"%{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release",
		monoInstallDir .. "/lib",
	}
	postbuildcommands
	{
		-- Assimp
		("{COPY} %{uwpAppIncludeDirs.Engine_ThirdParty}/assimp-3.3.1/build/code/Release/assimp-vc140-mt.dll ../Binaries/"..outputdir.."/Engine"),
		-- DX11 Debug Layers	
		("{COPY} %{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX11/Bin/D3D11SDKLayers.dll ../Binaries/"..outputdir.."/Engine"),
		("{COPY} %{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX11/Bin/D3DX11d_43.dll ../Binaries/"..outputdir.."/Engine"),
		("{COPY} %{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX11/Bin/D3D11Ref.dll ../Binaries/"..outputdir.."/Engine"),
		-- Mono					
		("{COPY} %{uwpAppIncludeDirs.Engine_ThirdParty}/Mono/bin/mono-2.0-sgen.dll ../Binaries/"..outputdir.."/Engine"),
		-- DirectX				
		("{COPY} %{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX12/Bin/dxcompiler.dll ../Binaries/"..outputdir.."/Engine"),
		("{COPY} %{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX12/Bin/dxil.dll ../Binaries/"..outputdir.."/Engine"),
		("{COPY} %{wks.location}/Engine/Shaders/HLSL/Ray_Tracing/** ../Binaries/" .. outputdir.."/Engine"),
		-- PIX					
		("{COPY} %{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/WinPixEventRuntime/bin/x64/WinPixEventRuntime_UAP.dll ../Binaries/"..outputdir.."/Engine"),
		-- Copy over assets
		("{COPY} $(USERPROFILE)/Documents/Insight-Projects/Development-Project/Content/** ../Binaries/" .. outputdir .. "/Content"),
		("{COPY} %{wks.location}/Engine/Assets/Textures/Default_Object/** ../Binaries/"..outputdir.."/Content/Default_Assets/")
	}

	filter "configurations:ShippingGame"
	libdirs
	{
		"%{uwpAppIncludeDirs.Engine_ThirdParty}/assimp-3.3.1/build/code/Release",            
		"%{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/WinPixEventRuntime/bin/x64",
		"%{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release",
		"%{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX12/DXTex/DirectXTex/Bin/Desktop_2019_Win10/x64/Release",
		"%{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release",
		monoInstallDir .. "/lib",
	}
	postbuildcommands
	{
		-- assimp
		("{COPY} %{uwpAppIncludeDirs.Engine_ThirdParty}/assimp-3.3.1/build/code/Release/assimp-vc140-mt.dll ../Binaries/"..outputdir.."/Engine"),
		-- mono
		("{COPY} %{uwpAppIncludeDirs.Engine_ThirdParty}/Mono/bin/mono-2.0-sgen.dll ../Binaries/"..outputdir.."/Engine"),
		-- DirectX
		("{COPY} %{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX12/Bin/dxcompiler.dll ../Binaries/"..outputdir.."/Engine"),
		("{COPY} %{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/DirectX12/Bin/dxil.dll ../Binaries/"..outputdir.."/Engine"),
		("{COPY} %{wks.location}/Engine/Shaders/HLSL/Ray_Tracing/** ../Binaries/" .. outputdir.."/Engine"),
		("{COPY} %{wks.location}/Engine/Assets/Textures/Default_Object/** ../Binaries/"..outputdir.."/Content/Default_Assets/")
	}

dofile ("../../Engine/BuildRules/Common-Build-Config.lua")
