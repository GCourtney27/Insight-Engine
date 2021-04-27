-- Build Rules for Engine Source Code
-- BuildRules projects take the Engine project and define variables 
-- and build parameters to make a library from. 


engineDirPath	= "../"
platform = "Win32"
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

project ("EngineBuild_Win32")
	location (engineDirPath)
	kind ("StaticLib")
	language ("C++")
	cppdialect ("C++17")
	staticruntime ("off")
	systemversion ("latest")
	targetname ("%{prj.name}")
	
	targetdir (ieGetBuildFolder() .. "%{prj.name}")
	objdir (ieGetBuildIntFolder())
	debugdir ("%{cfg.targetdir}/")

	pchheader ("Engine_pch.h")
	pchsource ("PCH_Source/Engine_pch.cpp")

	files
	{
		-- This Project's Make File
		"Build-Win32-Make.lua",

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
		-- Tells the engine to compile for Win32 platform
		"IE_PLATFORM_BUILD_WIN32=1",
		"_CRT_SECURE_NO_WARNINGS",
		"TRACK_RENDER_EVENTS=1", 
		"IE_WITH_DXR",
	}

	includedirs
	{
		-- Third Party
		"%{engineIncludeDirs.Microsoft}",
        "%{engineIncludeDirs.Microsoft}/WinPixEventRuntime/Include/",
		"%{engineIncludeDirs.Microsoft}DirectX12/",
		"%{engineIncludeDirs.Nvidia}DirectX12/",
		"%{engineIncludeDirs.rapidjson}include/",
		--"%{engineIncludeDirs.ImGuizmo}",
		"%{engineIncludeDirs.Mono}",
		"%{engineIncludeDirs.ImGui}",
		"%{engineIncludeDirs.assimp}",

		-- Engine Source code
		"%{engineIncludeDirs.Engine}/Source/",

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

	--premake.override(premake.vstudio.vc2010, "importExtensionTargets", function (oldfn, prj)
	--  oldfn(prj)
	--  premake.w('<Import Project="$(SolutionDir)\\Engine\\Tools\\MSBuildTools\\dxc.targets" />')
	--end)

dofile ("Common-Build-Config.lua")
