workspace "InsightEngine"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Engine-Dist",
		"Game-Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

gameName = "Application"

IncludeDir = {}
IncludeDir["ImGui"] = "Engine/vendor/imgui"

include "Engine/vendor/ImGui"

project ("Engine")
	location ("Engine")
	kind "WindowedApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "ie_pch.h"
	pchsource "Engine/src/ie_pch.cpp" 

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/vendor/Microsoft/DirectX12",
		"%{prj.name}/vendor/Nvidia/DirectX12",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.ImGui}/",
		"%{prj.name}/src/",
		gameName .. "/src/"
	}

	links
	{
		"d3d12.lib",
		"dxgi.lib",
		"d3dcompiler.lib",
		"ImGui",
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"IE_PLATFORM_WINDOWS"
		}
	
	-- Engine Development
	filter "configurations:Debug"
		defines "IE_DEBUG"
		runtime "Debug"
		symbols "on"
	
	-- Engine Release
	filter "configurations:Release"
		defines "IE_RELEASE"
		runtime "Release"
		optimize "on"
		symbols "on"
	
	-- Full Engine Distribution, all performance logs and debugging windows stripped
	filter "configurations:EngineDist"
		defines "IE_ENGINE_DIST"
		runtime "Release"
		optimize "on"
	
	-- Full Game Distribution, all engine debug tools(leel editors, editor user interfaces) stripped
	filter "configurations:GameDist"
		defines "IE_GAME_DIST"
		runtime "Release"
		optimize "on"

project (gameName)
	location (gameName)
	--[[
		TODO:   Change kind to ConsoleApp when using Vulkan
				or change kind to WindowedApp when using DX12
	--]]
	kind "StaticLib"
	cppdialect "C++17"
	language "C++"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
		"Engine/vendor/Microsoft/DirectX12",
		"Engine/vendor/Nvidia/DirectX12",
		"Engine/vendor/spdlog/include",
		"Engine/src",
		"Engine/vendor"
	}

	links
	{
		"Engine"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"IE_PLATFORM_WINDOWS",
			"IE_BUILD_DLL"
		}

	filter "configurations:Debug"
		defines "IE_DEBUG"
		symbols "on"
	
	filter "configurations:Release"
		defines "IE_RELEASE"
		symbols "on"
		optimize "on"

	filter "configurations:EngineDist"
		defines "IE_ENGINE_DIST"
		optimize "on"
		
	filter "configurations:GameDist"
		defines "IE_GAME_DIST"
		optimize "on"
