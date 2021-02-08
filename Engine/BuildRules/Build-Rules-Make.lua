-- Build Rules for Engine Source Code
-- BuildRules projects take the Engine project and define variables 
-- and build parameters to make a library from. 


rootDirPath		= "../../"
engineDirPath	= "../"
-- Assuming Mono is installed on the computer in the default diretory.
monoInstallDir	= "C:/Program Files/Mono/"

engineIncludeDirs = {}
engineIncludeDirs["ImGui"] 	    		= engineDirPath .. "ThirdParty/imgui/"
engineIncludeDirs["ImGuizmo"] 			= engineDirPath .. "ThirdParty/ImGuizmo/"
engineIncludeDirs["Microsoft"] 			= engineDirPath .. "ThirdParty/Microsoft/"
engineIncludeDirs["rapidjson"]  		= engineDirPath .. "ThirdParty/rapidjson/"
engineIncludeDirs["spdlog"] 			= engineDirPath .. "ThirdParty/spdlog/"
engineIncludeDirs["Mono"] 				= monoInstallDir .. "include/mono-2.0/"
engineIncludeDirs["assimp"] 			= engineDirPath .. "ThirdParty/assimp-5.0.1/include/"
engineIncludeDirs["OpenFBX"]			= engineDirPath .. "ThirdParty/OpenFBX/src/"
engineIncludeDirs["tinyobjloader"]		= engineDirPath .. "ThirdParty/tinyobjloader/include/"
engineIncludeDirs["Nvidia"] 			= engineDirPath .. "ThirdParty/Nvidia/"
engineIncludeDirs["Engine"] 			= engineDirPath

project ("EngineBuild_UWP")
	location (engineDirPath .. "BuildRules")
	kind ("StaticLib")
	language ("C++")
	cppdialect ("C++17")
	staticruntime ("off")
	systemversion ("latest")
	targetname ("%{prj.name}")

	targetdir (rootDirPath .. binaryDirectory .. "/%{prj.name}")
    objdir (rootDirPath .. intDirectory .. "/%{prj.name}")

	platforms { "x64" }
	defaultlanguage ("en-US")
	system  ("windowsuniversal")
	consumewinrtextension ("false")
	generatewinmd ("false")

	pchheader ("Engine_pch.h")
	pchsource ("PCH_Source/Engine_pch.cpp")

	files
	{
		-- This Project's Make File
		"Build-Rules-Make.lua",

		-- PCH for Engine Source Build
		"PCH_Source/**.h",
		"PCH_Source/**.cpp",

		-- Engine
		"%{engineIncludeDirs.Engine}/ThirdParty/Vendor_Build.cpp",
		"%{engineIncludeDirs.Engine}/Source/**.cpp",
		"%{engineIncludeDirs.Engine}/Source/**.h",
		"%{engineIncludeDirs.Engine}/Shaders/**.vertex.hlsl",
		"%{engineIncludeDirs.Engine}/Shaders/**.pixel.hlsl",
		"%{engineIncludeDirs.Engine}/Shaders/**.compute.hlsl",
	}

	defines
	{
		-- Tells the engine to compile for UWP platform
		"IE_PLATFORM_BUILD_UWP=1",
		"_CRT_SECURE_NO_WARNINGS",
	}

	includedirs
	{
		-- Third Party
		"%{engineIncludeDirs.assimp}",
		"%{engineIncludeDirs.OpenFBX}",
		"%{engineIncludeDirs.Microsoft}",
		"%{engineIncludeDirs.tinyobjloader}",
        "%{engineIncludeDirs.Microsoft}/WinPixEventRuntime/Include/",
		"%{engineIncludeDirs.Microsoft}DirectX12/",
		"%{engineIncludeDirs.Nvidia}DirectX12/",
		"%{engineIncludeDirs.rapidjson}include/",
		"%{engineIncludeDirs.spdlog}include/",
		"%{engineIncludeDirs.ImGui}",
		--"%{engineIncludeDirs.ImGuizmo}",

		-- Engine Source code
		"%{engineIncludeDirs.Engine}/Source/",
		"%{engineIncludeDirs.Engine}/Shaders/",

		-- This Projects PCH
		"PCH_Source/",

	}

	links
	{
        "ImGui",
	}

	flags
	{
		"MultiProcessorCompile"
	}

	postbuildcommands
	{
		-- Compile the ray tracing shaders.
		("%{wks.location}Engine/Shaders/HLSL/RayTracing/CompileRTShaders.bat %{wks.location}Binaries/" .. outputdir .. "/%{prj.name}"),
	}

	-- Shaders
	filter { "files:**.pixel.hlsl" }
		shadertype "Pixel"
		shadermodel "5.0"

	filter { "files:**.vertex.hlsl" }
		shadertype "Vertex"
		shadermodel "5.0"

	filter { "files:**.compute.hlsl" }
		shadertype "Compute"
		shadermodel "5.0"



	-- Engine Development
	filter "configurations:Debug*"
		defines "IE_DEBUG"
		runtime "Debug"
		symbols "on"
		defines
        {
            "IE_DEBUG"
        }

	-- Engine Release
	filter "configurations:Release*"
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
	filter "configurations:Dist*"
		defines "IE_GAME_DIST"
		runtime "Release"
		optimize "on"
		symbols "on"
		defines
		{
			"IE_DISTRIBUTION"
		}





project ("EngineBuild_Win32")
	location (engineDirPath .. "BuildRules")
	kind ("StaticLib")
	language ("C++")
	cppdialect ("C++17")
	staticruntime ("off")
	systemversion ("latest")
	targetname ("%{prj.name}")

	targetdir (rootDirPath .. binaryDirectory .. "%{prj.name}")
    objdir (rootDirPath .. intDirectory .. "%{prj.name}")

	pchheader ("Engine_pch.h")
	pchsource ("PCH_Source/Engine_pch.cpp")

	files
	{
		-- This Project's Make File
		"Build-Rules-Make.lua",

		-- PCH for Engine Source Build
		"PCH_Source/**.h",
		"PCH_Source/**.cpp",
		
		-- Engine
		"%{engineIncludeDirs.Engine}/ThirdParty/Vendor_Build.cpp",
		"%{engineIncludeDirs.Engine}/Source/**.cpp",
		"%{engineIncludeDirs.Engine}/Source/**.h",
		"%{engineIncludeDirs.Engine}/Shaders/**.vertex.hlsl",
		"%{engineIncludeDirs.Engine}/Shaders/**.pixel.hlsl",
		"%{engineIncludeDirs.Engine}/Shaders/**.compute.hlsl",
	}

	defines
	{
		-- Tells the engine to compile for Win32 platform
		"IE_PLATFORM_BUILD_WIN32=1",
		"_CRT_SECURE_NO_WARNINGS",
	}

	includedirs
	{
		-- Third Party
		"%{engineIncludeDirs.Microsoft}",
        "%{engineIncludeDirs.Microsoft}/WinPixEventRuntime/Include/",
		"%{engineIncludeDirs.Microsoft}DirectX12/",
		"%{engineIncludeDirs.Nvidia}DirectX12/",
		"%{engineIncludeDirs.rapidjson}include/",
		"%{engineIncludeDirs.spdlog}include/",
		--"%{engineIncludeDirs.ImGuizmo}",
		"%{engineIncludeDirs.Mono}",
		"%{engineIncludeDirs.ImGui}",
		"%{engineIncludeDirs.assimp}",

		-- Engine Source code
		"%{engineIncludeDirs.Engine}/Source/",
		"%{engineIncludeDirs.Engine}/Shaders/",

		-- This Projects PCH
		"PCH_Source/",
	}

	links
	{
        "ImGui",
        --"Engine"
	}

	flags
	{
		"MultiProcessorCompile"
	}

	postbuildcommands
	{
		-- Compile the ray tracing shaders.
		("%{wks.location}Engine/Shaders/HLSL/RayTracing/CompileRTShaders.bat %{wks.location}Binaries/" .. outputdir .. "/%{prj.name}"),

	}

	-- Shaders
	filter { "files:**.pixel.hlsl" }
		shadertype "Pixel"
		shadermodel "5.0"

	filter { "files:**.vertex.hlsl" }
		shadertype "Vertex"
		shadermodel "5.0"

	filter { "files:**.compute.hlsl" }
		shadertype "Compute"
		shadermodel "5.0"


	-- Engine Development
	filter "configurations:Debug*"
		defines "IE_DEBUG"
		runtime "Debug"
		symbols "on"
		defines
        {
            "IE_DEBUG"
        }

	-- Engine Release
	filter "configurations:Release*"
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
	filter "configurations:Dist*"
		defines "IE_GAME_DIST"
		runtime "Release"
		optimize "on"
		symbols "on"
		defines
		{
			"IE_DISTRIBUTION"
		}

