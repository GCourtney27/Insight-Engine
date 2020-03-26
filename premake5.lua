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

project "Engine"
	location "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "%/{prj.name}")
	objdir ("bin-int/" .. outputdir .. "%/{prj.name}")

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
		--"%{prj.name}/vendor/spdlog/include"
		"%{prj.name}/src/"
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
		runtime "Debug"
		symbols "on"
	
	filter "configurations:Release"
		defines "IE_RELEASE"
		runtime "Release"
		optimize "on"
		symbols "on"

	filter "configurations:EngineDist"
		defines "IE_ENGINE_DIST"
		runtime "Release"
		optimize "on"
		
	filter "configurations:GameDist"
		defines "IE_GAME_DIST"
		runtime "Release"
		optimize "on"

project "Application"
	location "Application"
	--[[
		TODO:   Change kind to ConsoleApp when using Vulkan
				or change kind to WindowedApp when using DX12
	--]]
	kind "WindowedApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "%/{prj.name}")
	objdir ("bin-int/" .. outputdir .. "%/{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
		--"%{prj.name}/vendor/spdlog/include",
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
			"IE_PLATFORM_WINDOWS"
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
