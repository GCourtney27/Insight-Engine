-- Engine

rootDirectoryPath = "../"

EngineIncludeDirs = {}
EngineIncludeDirs["ImGui"] 	    = "Vendor/imgui"
EngineIncludeDirs["ImGuizmo"] 	= "Vendor/ImGuizmo"
EngineIncludeDirs["Microsoft"] 	= "Vendor/Microsoft"
EngineIncludeDirs["rapidjson"]  = "Vendor/rapidjson"
EngineIncludeDirs["spdlog"] 	= "Vendor/spdlog"
EngineIncludeDirs["Mono"] 		= "Vendor/Mono/include/mono-2.0"
EngineIncludeDirs["assimp"] 	= "Vendor/assimp-3.3.1/include"
EngineIncludeDirs["Modules"] 	= "Modules"
EngineIncludeDirs["Nvidia"] 	= "Vendor/Nvidia"

vendorDir = "%{wks.location}/Engine/Vendor/"

project ("Engine")
	location (rootDirectoryPath .. "Engine")
	kind "WindowedApp"
	-- kind "ConsoleApp"
	--[[
	TODO:   Change kind to ConsoleApp when using Vulkan
			or change kind to WindowedApp when using DX12
	--]]
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir (rootDirectoryPath .. "Bin/" .. outputdir .. "/%{prj.name}")
	objdir (rootDirectoryPath .. "Bin-Int/" .. outputdir .. "/%{prj.name}")

	pchheader ("Engine_pch.h")
	pchsource ("Source/Engine_pch.cpp")

	files
	{
		"Engine-Make.lua",
		"Vendor/Vendor_Build.cpp",
		"Source/**.cpp",
		"Source/**.h",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"IE_BUILD_DIR=%{CustomDefines.IE_BUILD_DIR}/Engine/",
		"IE_BUILD_CONFIG=%{CustomDefines.IE_BUILD_CONFIG}"
	}

	includedirs
	{
		-- Vendor
		"%{EngineIncludeDirs.Microsoft}/",
        "%{EngineIncludeDirs.Microsoft}/DirectX12/WinPixEventRuntime.1.0.161208001/Include/",
		"%{EngineIncludeDirs.Microsoft}/DirectX12",
		"%{EngineIncludeDirs.Nvidia}/DirectX12/",
		"%{EngineIncludeDirs.rapidjson}/include/",
		"%{EngineIncludeDirs.spdlog}/include/",
		"%{EngineIncludeDirs.ImGuizmo}/",
		"%{EngineIncludeDirs.Mono}/",
		"%{EngineIncludeDirs.ImGui}/",
		"%{EngineIncludeDirs.assimp}/",
		
		-- Modules
        "%{EngineIncludeDirs.Modules}/Renderer/Source/",
		
		-- Engine
		"Source/",
		-- Application
		rootDirectoryPath .. gameName .. "/Source/"
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
        
        "ImGui",
        "Renderer"
	}

	filter {"system:windows"}
		systemversion "latest"

		defines
		{
			"IE_PLATFORM_WINDOWS"
		}

		flags
		{
			"MultiProcessorCompile"
		}
		
	-- End filter - windows

	-- Engine Development
	filter "configurations:Debug"
		defines "IE_DEBUG"
		runtime "Debug"
		symbols "on"
		defines
        {
            "IE_DEBUG"
        }
		libdirs
		{
            "Vendor/assimp-3.3.1/build/code/Debug",
            "Vendor/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/",
            "Vendor/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Debug",
            "Vendor/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Debug",
			"Vendor/Mono/lib",
		}
		postbuildcommands
		{
			-- Assimp
			("{COPY} ".. vendorDir .."/assimp-3.3.1/build/code/Debug/assimp-vc140-mt.dll ../bin/"..outputdir.."/Engine"),
			-- DX11 Debug Layers
			("{COPY} ".. vendorDir .."/Microsoft/DirectX11/Bin/D3D11SDKLayers.dll ../bin/"..outputdir.."/Engine"),
			("{COPY} ".. vendorDir .."/Microsoft/DirectX11/Bin/D3DX11d_43.dll ../bin/"..outputdir.."/Engine"),
			("{COPY} ".. vendorDir .."/Microsoft/DirectX11/Bin/D3D11Ref.dll ../bin/"..outputdir.."/Engine"),
			-- Mono
			("{COPY} ".. vendorDir .."/Mono/bin/mono-2.0-sgen.dll ../bin/"..outputdir.."/Engine"),
			-- DirectX
			("{COPY} ".. vendorDir .."/Microsoft/DirectX12/Bin/dxcompiler.dll ../bin/"..outputdir.."/Engine"),
			("{COPY} ".. vendorDir .."/Microsoft/DirectX12/Bin/dxil.dll ../bin/"..outputdir.."/Engine"),
			-- PIX
			("{COPY} ".. vendorDir .."/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/WinPixEventRuntime.dll ../bin/"..outputdir.."/Engine"),
			("{COPY} ".. vendorDir .."/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/WinPixEventRuntime_UAP.dll ../bin/"..outputdir.."/Engine"),
			-- DXR Shaders
			("{COPY} %{wks.location}/Engine/Modules/Renderer/Source/Renderer/Shaders/HLSL/Ray_Tracing/** ../bin/"..outputdir.."/Renderer")
		}
	-- Engine Release
	filter "configurations:Release"
		defines "IE_RELEASE"
		runtime "Release"
		optimize "on"
		symbols "on"
		defines
		{
			"IE_IS_STANDALONE",
			"IE_DEBUG"
		}
		libdirs
		{
			"Vendor/assimp-3.3.1/build/code/Release",
            "Vendor/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/",
            "Vendor/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release",
            "Vendor/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release",
			"Vendor/Mono/lib",
		}
		postbuildcommands
		{
			-- Assimp
			("{COPY} ".. vendorDir .."/assimp-3.3.1/build/code/Release/assimp-vc140-mt.dll ../bin/"..outputdir.."/Engine"),
			-- DX11 Debug Layers	
			("{COPY} ".. vendorDir .."/Microsoft/DirectX11/Bin/D3D11SDKLayers.dll ../bin/"..outputdir.."/Engine"),
			("{COPY} ".. vendorDir .."/Microsoft/DirectX11/Bin/D3DX11d_43.dll ../bin/"..outputdir.."/Engine"),
			("{COPY} ".. vendorDir .."/Microsoft/DirectX11/Bin/D3D11Ref.dll ../bin/"..outputdir.."/Engine"),
			-- Mono					
			("{COPY} ".. vendorDir .."/Mono/bin/mono-2.0-sgen.dll ../bin/"..outputdir.."/Engine"),
			-- DirectX				
			("{COPY} ".. vendorDir .."/Microsoft/DirectX12/Bin/dxcompiler.dll ../bin/"..outputdir.."/Engine"),
			("{COPY} ".. vendorDir .."/Microsoft/DirectX12/Bin/dxil.dll ../bin/"..outputdir.."/Engine"),
			-- PIX					
			("{COPY} ".. vendorDir .."/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/WinPixEventRuntime.dll ../bin/"..outputdir.."/Engine"),
			("{COPY} ".. vendorDir .."/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/WinPixEventRuntime_UAP.dll ../bin/"..outputdir.."/Engine"),
			-- DXR Shaders
			("{COPY} %{wks.location}/Engine/Modules/Renderer/Source/Renderer/Shaders/HLSL/Ray_Tracing/** ../bin/"..outputdir.."/Renderer")
		}





	-- Full Engine Distribution, all performance logs and debugging windows stripped
	filter "configurations:Engine-Dist"
		defines "IE_ENGINE_DIST"
		runtime "Release"
		optimize "on"
		symbols "on"
		defines
		{
			"IE_IS_STANDALONE"
		}
		libdirs
		{
			"Vendor/assimp-3.3.1/build/code/Release",
            "Vendor/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/",
            "Vendor/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release",
            "Vendor/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release",
			"Vendor/Mono/lib",
		}
		postbuildcommands
		{
			-- assimp
			("{COPY} ".. vendorDir .."/assimp-3.3.1/build/code/Release/assimp-vc140-mt.dll ../bin/"..outputdir.."/Engine"),
			-- mono
			("{COPY} ".. vendorDir .."/Mono/bin/mono-2.0-sgen.dll ../bin/"..outputdir.."/Engine"),
			-- DirectX
			("{COPY} ".. vendorDir .."/Microsoft/DirectX12/Bin/dxcompiler.dll ../bin/"..outputdir.."/Engine"),
			("{COPY} ".. vendorDir .."/Microsoft/DirectX12/Bin/dxil.dll ../bin/"..outputdir.."/Engine"),
			-- DXR Shaders
			("{COPY} %{wks.location}/Engine/Modules/Renderer/Source/Renderer/Shaders/HLSL/Ray_Tracing/** ../bin/"..outputdir.."/Renderer")
		}
	-- Full Game Distribution, all engine debug tools(level editors, editor user interfaces) stripped
	filter "configurations:Game-Dist"
		defines "IE_GAME_DIST"
		runtime "Release"
		optimize "on"
		symbols "on"
		defines
		{
			"IE_IS_STANDALONE"
		}
		libdirs
		{
            "Vendor/assimp-3.3.1/build/code/Release",            
            "Vendor/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/",
            "Vendor/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release",
            "Vendor/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release",
			"Vendor/Mono/lib",
		}
		postbuildcommands
		{
			-- assimp
			("{COPY} ".. vendorDir .."/assimp-3.3.1/build/code/Release/assimp-vc140-mt.dll ../bin/"..outputdir.."/Engine"),
			-- mono
			("{COPY} ".. vendorDir .."/Mono/bin/mono-2.0-sgen.dll ../bin/"..outputdir.."/Engine"),
			-- DirectX
			("{COPY} ".. vendorDir .."/Microsoft/DirectX12/Bin/dxcompiler.dll ../bin/"..outputdir.."/Engine"),
			("{COPY} ".. vendorDir .."/Microsoft/DirectX12/Bin/dxil.dll ../bin/"..outputdir.."/Engine"),
			-- DXR Shaders
			("{COPY} %{wks.location}/Engine/Modules/Renderer/Source/Renderer/Shaders/HLSL/Ray_Tracing/** ../bin/"..outputdir.."/Renderer")
		}
