-- Engine

rootDirPath = "../"
monoInstallDir = "C:/Program Files/Mono/"

--EngineIncludeDirs = {}
--EngineIncludeDirs["ImGui"] 	    = "Third_Party/imgui"
--EngineIncludeDirs["ImGuizmo"] 	= "Third_Party/ImGuizmo"
--EngineIncludeDirs["Microsoft"] 	= "Third_Party/Microsoft"
--EngineIncludeDirs["rapidjson"]  = "Third_Party/rapidjson"
--EngineIncludeDirs["spdlog"] 	= "Third_Party/spdlog"
--EngineIncludeDirs["Mono"] 		= monoInstallDir .. "/include/mono-2.0"
--EngineIncludeDirs["assimp"] 	= "Third_Party/assimp-3.3.1/include"
--EngineIncludeDirs["Nvidia"] 	= "Third_Party/Nvidia"


project ("Engine_Source")
	location (rootDirPath .. "Engine_Source")
	kind "SharedItems"
	language "C++"
	cppdialect "C++17"
	--staticruntime "off"
	--targetname("InsightEngine_%{cfg.system}")
	
	--targetdir (rootDirPath .. "Binaries/" .. outputdir .. "/%{prj.name}")
    --objdir (rootDirPath .. "Binaries/Intermediates/" .. outputdir .. "/%{prj.name}")

	--pchheader ("Engine_pch.h")
	--pchsource ("Source/Engine_pch.cpp")
	
	-- files
	-- {
	-- 	"Engine-Make.lua",
	-- 	"Third_Party/Vendor_Build.cpp",
	-- 	"Source/**.cpp",
	-- 	"Source/**.h",
	-- 	"Source/**.vertex.hlsl",
	-- 	"Source/**.pixel.hlsl",
	-- 	"Source/**.compute.hlsl",
	-- }
	--
	--defines
	--{
	--	"_CRT_SECURE_NO_WARNINGS",
	--	"IE_BUILD_DIR=%{CustomDefines.IE_BUILD_DIR}/Engine/",
	--	"IE_BUILD_CONFIG=%{CustomDefines.IE_BUILD_CONFIG}",
	--}
	--
	-- includedirs
	-- {
	-- 	-- Third Party
	-- 	"%{EngineIncludeDirs.Microsoft}/",
    --     "%{EngineIncludeDirs.Microsoft}/DirectX12/WinPixEventRuntime.1.0.161208001/Include/",
	-- 	"%{EngineIncludeDirs.Microsoft}/DirectX12",
	-- 	"%{EngineIncludeDirs.Nvidia}/DirectX12/",
	-- 	"%{EngineIncludeDirs.rapidjson}/include/",
	-- 	"%{EngineIncludeDirs.spdlog}/include/",
	-- 	--"%{EngineIncludeDirs.ImGuizmo}/",
	-- 	"%{EngineIncludeDirs.Mono}/",
	-- 	"%{EngineIncludeDirs.ImGui}/",
	-- 	"%{EngineIncludeDirs.assimp}/",
		
	-- 	-- Engine
	-- 	"Source/",
	-- }
	--
	--links
	--{
    --    "ImGui",
    --    
	--}
	--
	--
	--
	--filter { "system:windows" }
	--	systemversion "latest"
	--
	--	defines
	--	{
	--		"IE_PLATFORM_BUILD_WIN32",
	--	}
	--
	--	flags
	--	{
	--		"MultiProcessorCompile"
	--	}
	--	
		-- Shaders
        -- filter { "files:**.pixel.hlsl" }
        --     shadertype "Pixel"
        --     shadermodel "5.0"
        
        -- filter { "files:**.vertex.hlsl" }
        --     shadertype "Vertex"
        --     shadermodel "5.0"
	
        -- filter { "files:**.compute.hlsl" }
        --     shadertype "Compute"
        --     shadermodel "5.0"
	
	-- End filter - windows
	--
	--
	--
	---- Engine Development
	--filter "configurations:Debug"
	--	defines "IE_DEBUG"
	--	runtime "Debug"
	--	symbols "on"
	--	defines
    --    {
    --        "IE_DEBUG"
    --    }
	--
	---- Engine Release
	--filter "configurations:Release"
	--	defines "IE_RELEASE"
	--	runtime "Release"
	--	optimize "on"
	--	symbols "on"
	--	defines
	--	{
	--		"IE_DEPLOYMENT",
	--		"IE_DEBUG"
	--	}
	--
	--
	--
	---- Full Engine Distribution, all performance logs and debugging windows stripped
	--filter "configurations:Engine-Dist"
	--	defines "IE_ENGINE_DIST"
	--	runtime "Release"
	--	optimize "on"
	--	symbols "on"
	--	defines
	--	{
	--		"IE_DISTRIBUTION"
	--	}
	---- Full Game Distribution, all engine debug tools(level editors, editor user interfaces) stripped
	--filter "configurations:Game-Dist"
	--	defines "IE_GAME_DIST"
	--	runtime "Release"
	--	optimize "on"
	--	symbols "on"
	--	defines
	--	{
	--		"IE_DISTRIBUTION"
	--	}
	--
	--