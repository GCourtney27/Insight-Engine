-- Build Rules for Engine Source Code
-- Build_Rules projects take the Engine_Source project and define variables 
-- and build parameters to make a library from. 


rootDirPath		= "../"
engineDirPath	= rootDirPath .. "Engine_Source/"
-- Assuming Mono is installed on the computer in the default diretory.
monoInstallDir	= "C:/Program Files/Mono/"

engineIncludeDirs = {}
engineIncludeDirs["OpenFBX"]		= engineDirPath .. "Third_Party/OpenFBX/src/"
engineIncludeDirs["ImGui"] 	    	= engineDirPath .. "Third_Party/imgui/"
engineIncludeDirs["ImGuizmo"] 		= engineDirPath .. "Third_Party/ImGuizmo/"
engineIncludeDirs["Microsoft"] 		= engineDirPath .. "Third_Party/Microsoft/"
engineIncludeDirs["rapidjson"]  	= engineDirPath .. "Third_Party/rapidjson/"
engineIncludeDirs["spdlog"] 		= engineDirPath .. "Third_Party/spdlog/"
engineIncludeDirs["Mono"] 			= monoInstallDir .. "include/mono-2.0/"
engineIncludeDirs["assimp"] 		= engineDirPath .. "Third_Party/assimp-3.3.1/include/"
engineIncludeDirs["Nvidia"] 		= engineDirPath .. "Third_Party/Nvidia/"
engineIncludeDirs["Engine_Source"] 	= rootDirPath .. "Engine_Source/"

-- Premake does not support UWP project generation
-- So just add the Visual Studio created one.
project ("Engine_Build_UWP")
	location (rootDirPath .. "Build_Rules")
	kind ("StaticLib")
	language ("C++")
	cppdialect ("C++17")
	staticruntime ("off")
	systemversion ("latest")
	targetname ("%{prj.name}")

	targetdir (rootDirPath .. "Binaries/" .. outputdir .. "/%{prj.name}")
    objdir (rootDirPath .. "Binaries/Intermediates/" .. outputdir .. "/%{prj.name}")

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

		"%{engineIncludeDirs.Engine_Source}/Third_Party/Vendor_Build.cpp",
		"%{engineIncludeDirs.Engine_Source}/Source/**.cpp",
		"%{engineIncludeDirs.Engine_Source}/Source/**.h",
		"%{engineIncludeDirs.Engine_Source}/Source/**.vertex.hlsl",
		"%{engineIncludeDirs.Engine_Source}/Source/**.pixel.hlsl",
		"%{engineIncludeDirs.Engine_Source}/Source/**.compute.hlsl",
	}

	defines
	{
		-- Tells the Engine to Compile for Win32 Platform
		"IE_PLATFORM_BUILD_UWP",
		"_CRT_SECURE_NO_WARNINGS",
		"IE_BUILD_DIR=%{CustomDefines.IE_BUILD_DIR}/${prj.name}/",
		"IE_BUILD_CONFIG=%{CustomDefines.IE_BUILD_CONFIG}",
	}

	includedirs
	{
		-- Third Party
		"%{engineIncludeDirs.OpenFBX}",
		"%{engineIncludeDirs.Microsoft}",
        "%{engineIncludeDirs.Microsoft}DirectX12/WinPixEventRuntime.1.0.161208001/Include/",
		"%{engineIncludeDirs.Microsoft}DirectX12/",
		"%{engineIncludeDirs.Nvidia}DirectX12/",
		"%{engineIncludeDirs.rapidjson}include/",
		"%{engineIncludeDirs.spdlog}include/",
		--"%{engineIncludeDirs.ImGuizmo}",
		"%{engineIncludeDirs.assimp}",

		-- Engine Source code
		"%{engineIncludeDirs.Engine_Source}/Source/",

		-- This Projects PCH
		"PCH_Source/"
	}

	links
	{
        "Engine_Source"
	}

	flags
	{
		"MultiProcessorCompile"
	}

	postbuildcommands
	{
		("{COPY} ../Engine_Source/Source/Shaders/HLSL/Ray_Tracing/** ../Binaries/" .. outputdir.."/%{prj.name}"),
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





project ("Engine_Build_Win32")
	location (rootDirPath .. "Build_Rules")
	kind ("StaticLib")
	language ("C++")
	cppdialect ("C++17")
	staticruntime ("off")
	systemversion ("latest")
	targetname ("%{prj.name}")

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

		"%{engineIncludeDirs.Engine_Source}/Third_Party/Vendor_Build.cpp",
		"%{engineIncludeDirs.Engine_Source}/Source/**.cpp",
		"%{engineIncludeDirs.Engine_Source}/Source/**.h",
		"%{engineIncludeDirs.Engine_Source}/Source/**.vertex.hlsl",
		"%{engineIncludeDirs.Engine_Source}/Source/**.pixel.hlsl",
		"%{engineIncludeDirs.Engine_Source}/Source/**.compute.hlsl",
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
		"%{engineIncludeDirs.Microsoft}",
        "%{engineIncludeDirs.Microsoft}DirectX12/WinPixEventRuntime.1.0.161208001/Include/",
		"%{engineIncludeDirs.Microsoft}DirectX12/",
		"%{engineIncludeDirs.Nvidia}DirectX12/",
		"%{engineIncludeDirs.rapidjson}include/",
		"%{engineIncludeDirs.spdlog}include/",
		--"%{engineIncludeDirs.ImGuizmo}",
		"%{engineIncludeDirs.Mono}",
		"%{engineIncludeDirs.ImGui}",
		"%{engineIncludeDirs.assimp}",

		-- Engine Source code
		"%{engineIncludeDirs.Engine_Source}/Source/",

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

	postbuildcommands
	{
		-- ("{COPY} ../Engine_Source/Source/Shaders/HLSL/Ray_Tracing/** ../Binaries/" .. outputdir.."/%{prj.name}"),
		("%{wks.location}Engine_Source/Source/Shaders/HLSL/Ray_Tracing/Compile_RT_Shaders.bat %{wks.location}Binaries/" .. outputdir .. "/%{prj.name}"),
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

