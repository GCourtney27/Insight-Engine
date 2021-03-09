
include ("Common-Build-Config.lua")


engineDirPath	= "../"
platform = "UWP"

-- Assuming Mono is installed on the computer in the default diretory.
monoInstallDir	= "C:/Program Files/Mono/"

engineIncludeDirs = {}
engineIncludeDirs["ImGui"] 	    		= engineDirPath .. "ThirdParty/imgui/"
engineIncludeDirs["ImGuizmo"] 			= engineDirPath .. "ThirdParty/ImGuizmo/"
engineIncludeDirs["Microsoft"] 			= engineDirPath .. "ThirdParty/Microsoft/"
engineIncludeDirs["rapidjson"]  		= engineDirPath .. "ThirdParty/rapidjson/"
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
	systemversion ("10.0.18362.0")
	targetname ("%{prj.name}")
	
	targetdir (ieGetBuildFolder())
	objdir (ieGetBuildIntFolder())

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
		"Build-UWP-Make.lua",

		-- PCH for Engine Source Build
		"PCH_Source/**.h",
		"PCH_Source/**.cpp",

		-- Engine
		"%{engineIncludeDirs.Engine}/ThirdParty/Vendor_Build.cpp",
		"%{engineIncludeDirs.Engine}/Source/**.cpp",
		"%{engineIncludeDirs.Engine}/Source/**.h",
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
		"%{engineIncludeDirs.ImGui}",

		-- Engine Source code
		"%{engineIncludeDirs.Engine}/Source/",

		-- This projects PCH
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


--Filters

	filter { "platforms:XboxOne" }
	defines
	{
		"IE_PLATFORM_BUILD_XBOX_ONE=1",
		"TRACK_RENDER_EVENTS=0",
	}
		

	filter { "platforms:UniversalWindowsDesktop" }
	defines
	{
		"TRACK_RENDER_EVENTS=1", 
	}

dofile ("Common-Build-Config.lua")
