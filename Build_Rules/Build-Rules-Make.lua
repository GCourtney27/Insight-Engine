-- Build Rules for Engine Source Code

rootDirPath		= "../"
engineDirPath	= rootDirPath .. "Engine_Source/"
monoInstallDir	= "C:/Program Files/Mono/"

EngineIncludeDirs = {}
EngineIncludeDirs["ImGui"] 	    = engineDirPath .. "Third_Party/imgui/"
EngineIncludeDirs["ImGuizmo"] 	= engineDirPath .. "Third_Party/ImGuizmo/"
EngineIncludeDirs["Microsoft"] 	= engineDirPath .. "Third_Party/Microsoft/"
EngineIncludeDirs["rapidjson"]  = engineDirPath .. "Third_Party/rapidjson/"
EngineIncludeDirs["spdlog"] 	= engineDirPath .. "Third_Party/spdlog/"
EngineIncludeDirs["Mono"] 		= monoInstallDir .. "/include/mono-2.0/"
EngineIncludeDirs["assimp"] 	= engineDirPath .. "Third_Party/assimp-3.3.1/include/"
EngineIncludeDirs["Nvidia"] 	= engineDirPath .. "Third_Party/Nvidia/"


project ("Engine_Build_UWP")
	location (rootDirPath .. "Build_Rules")
	kind ("SharedLib")
	language ("C++")
	cppdialect ("C++17")
	staticruntime ("off")
	targetname ("InsightEngine_%{cfg.system}")

	targetdir (rootDirPath .. "Binaries/" .. outputdir .. "/%{prj.name}")
    objdir (rootDirPath .. "Binaries/Intermediates/" .. outputdir .. "/%{prj.name}")
	
	pchheader ("Engine_pch.h")
	pchsource ("Source/Engine_pch.cpp")

	defines
	{
		"IE_PLATFORM_BUILD_UWP",

		"_CRT_SECURE_NO_WARNINGS",
		"IE_BUILD_DIR=%{CustomDefines.IE_BUILD_DIR}/Engine/",
		"IE_BUILD_CONFIG=%{CustomDefines.IE_BUILD_CONFIG}",
	}
	



project ("Engine_Build_Win32")
	location (rootDirPath .. "Build_Rules")
	kind ("StaticLib")
	language ("C++")
	cppdialect ("C++17")
	staticruntime ("off")
	systemversion ("latest")
	targetname ("InsightEngine_Win32")
	
	targetdir (rootDirPath .. "Binaries/" .. outputdir .. "/%{prj.name}")
    objdir (rootDirPath .. "Binaries/Intermediates/" .. outputdir .. "/%{prj.name}")
	
	pchheader ("Engine_pch.h")
	pchsource ("PCH_Source/Engine_pch.cpp")

	files
	{
		-- This Project's Make File
		"Build-Rules-Make.lua",

		-- PCH for Engine Source Build
		"PCH_Source/**.h",
		"PCH_Source/**.cpp",
	}

	defines
	{
		-- Tells the Engine to Compile for Win32 Platform
		"IE_PLATFORM_BUILD_WIN32",

		"_CRT_SECURE_NO_WARNINGS",
		"IE_BUILD_DIR=%{CustomDefines.IE_BUILD_DIR}/${prj.name}/",
		"IE_BUILD_CONFIG=%{CustomDefines.IE_BUILD_CONFIG}",
	}

	includedirs
	{
		-- Third Party
		"%{EngineIncludeDirs.Microsoft}",
        "%{EngineIncludeDirs.Microsoft}DirectX12/WinPixEventRuntime.1.0.161208001/Include/",
		"%{EngineIncludeDirs.Microsoft}DirectX12/",
		"%{EngineIncludeDirs.Nvidia}DirectX12/",
		"%{EngineIncludeDirs.rapidjson}include/",
		"%{EngineIncludeDirs.spdlog}include/",
		"%{EngineIncludeDirs.ImGuizmo}",
		"%{EngineIncludeDirs.Mono}",
		"%{EngineIncludeDirs.ImGui}",
		"%{EngineIncludeDirs.assimp}",
		
		-- Engine Source code
		engineDirPath .. "Source/",

		-- This Projects PCH
		"PCH_Source/"
	}

	links
	{
        "ImGui",
        "Engine_Source"
	}

	flags
	{
		"MultiProcessorCompile"
	}
		



	-- Engine Development
	filter "configurations:Debug"
		defines "IE_DEBUG"
		runtime "Debug"
		symbols "on"
		defines
        {
            "IE_DEBUG"
        }

	-- Engine Release
	filter "configurations:Release"
		defines "IE_RELEASE"
		runtime "Release"
		optimize "on"
		symbols "on"
		defines
		{
			"IE_DEPLOYMENT",
			"IE_DEBUG"
		}



	-- Full Engine Distribution, all performance logs and debugging windows stripped
	filter "configurations:Engine-Dist"
		defines "IE_ENGINE_DIST"
		runtime "Release"
		optimize "on"
		symbols "on"
		defines
		{
			"IE_DISTRIBUTION"
		}
	-- Full Game Distribution, all engine debug tools(level editors, editor user interfaces) stripped
	filter "configurations:Game-Dist"
		defines "IE_GAME_DIST"
		runtime "Release"
		optimize "on"
		symbols "on"
		defines
		{
			"IE_DISTRIBUTION"
		}

