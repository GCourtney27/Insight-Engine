-- Application

gameName = "Application"
rootDirectoryPath = "../"

project (gameName)
	location (rootDirectoryPath .. gameName)
	
	kind "StaticLib"
	cppdialect "C++17"
	language "C++"
	staticruntime "off"

	targetdir (rootDirectoryPath .. "Bin/" .. outputdir .. "/%{prj.name}")
	objdir (rootDirectoryPath .. "Bin-Int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"Source/**.h",
		"Source/**.cpp",
	}

	includedirs
	{
		rootDirectoryPath .. "Engine/Vendor/assimp-3.3.1/include",
		rootDirectoryPath .. "Engine/Vendor/Microsoft/",
		rootDirectoryPath .. "Engine/Vendor/Nvidia/DirectX12",
		rootDirectoryPath .. "Engine/Vendor/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/Include/",
		rootDirectoryPath .. "Engine/Vendor/spdlog/include",
		rootDirectoryPath .. "Engine/Vendor/rapidjson/include",
		rootDirectoryPath .. "Engine/Vendor/Mono/include/mono-2.0",
		rootDirectoryPath .. "Engine/Source",
		rootDirectoryPath .. "Engine/Vendor",
		rootDirectoryPath .. "Engine/Modules/Renderer/Source"
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
		symbols "on"
		
	filter "configurations:GameDist"
		defines "IE_GAME_DIST"
		optimize "on"
		symbols "on"

	