-- Build Rules for Engine Source Code
-- Build_Rules projects take the Engine project and define variables 
-- and build parameters to make a library from. 


rootDirPath		= "../"
engineDirPath	= rootDirPath .. "Engine/"
-- Assuming Mono is installed on the computer in the default diretory.
monoInstallDir	= "C:/Program Files/Mono/"

engineIncludeDirs = {}
engineIncludeDirs["ImGui"] 	    		= engineDirPath .. "Third_Party/imgui/"
engineIncludeDirs["ImGuizmo"] 			= engineDirPath .. "Third_Party/ImGuizmo/"
engineIncludeDirs["Microsoft"] 			= engineDirPath .. "Third_Party/Microsoft/"
engineIncludeDirs["rapidjson"]  		= engineDirPath .. "Third_Party/rapidjson/"
engineIncludeDirs["spdlog"] 			= engineDirPath .. "Third_Party/spdlog/"
engineIncludeDirs["Mono"] 				= monoInstallDir .. "include/mono-2.0/"
engineIncludeDirs["assimp"] 			= engineDirPath .. "Third_Party/assimp-5.0.1/include/"
engineIncludeDirs["OpenFBX"]			= engineDirPath .. "Third_Party/OpenFBX/src/"
engineIncludeDirs["tinyobjloader"]		= engineDirPath .. "Third_Party/tinyobjloader/include/"
engineIncludeDirs["Nvidia"] 			= engineDirPath .. "Third_Party/Nvidia/"
engineIncludeDirs["Engine"] 			= rootDirPath .. "Engine/"
engineIncludeDirs["Game_Runtime"]		= rootDirPath .. "Game_Runtime/Source/"

-- Premake does not support UWP project generation
-- So just add the Visual Studio created one.
project ("EngineBuild_UWP")
	location (rootDirPath .. "Build_Rules")
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
		"%{engineIncludeDirs.Engine}/Third_Party/Vendor_Build.cpp",
		"%{engineIncludeDirs.Engine}/Source/**.cpp",
		"%{engineIncludeDirs.Engine}/Source/**.h",
		"%{engineIncludeDirs.Engine}/Source/**.vertex.hlsl",
		"%{engineIncludeDirs.Engine}/Source/**.pixel.hlsl",
		"%{engineIncludeDirs.Engine}/Source/**.compute.hlsl",
	}

	defines
	{
		-- Tells the engine to compile for UWP platform
		"IE_PLATFORM_BUILD_UWP",
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

		-- This Projects PCH
		"PCH_Source/",

	}

	links
	{
        "ImGui",
        "Engine"
	}

	flags
	{
		"MultiProcessorCompile"
	}

	postbuildcommands
	{
		-- Compile the ray tracing shaders.
		("%{wks.location}Engine/Source/Shaders/HLSL/RayTracing/CompileRTShaders.bat %{wks.location}Binaries/" .. outputdir .. "/%{prj.name}"),
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
	location (rootDirPath .. "Build_Rules")
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
		"%{engineIncludeDirs.Engine}/Third_Party/Vendor_Build.cpp",
		"%{engineIncludeDirs.Engine}/Source/**.cpp",
		"%{engineIncludeDirs.Engine}/Source/**.h",
		"%{engineIncludeDirs.Engine}/Source/**.vertex.hlsl",
		"%{engineIncludeDirs.Engine}/Source/**.pixel.hlsl",
		"%{engineIncludeDirs.Engine}/Source/**.compute.hlsl",
	}

	defines
	{
		-- Tells the engine to compile for Win32 platform
		"IE_PLATFORM_BUILD_WIN32",
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

		-- This Projects PCH
		"PCH_Source/",

		-- Game runtime source
		"%{engineIncludeDirs.Game_Runtime}/",
	}

	links
	{
        "ImGui",
        "Engine"
	}

	flags
	{
		"MultiProcessorCompile"
	}

	postbuildcommands
	{
		-- Compile the ray tracing shaders.
		("%{wks.location}Engine/Source/Shaders/HLSL/RayTracing/CompileRTShaders.bat %{wks.location}Binaries/" .. outputdir .. "/%{prj.name}"),
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

