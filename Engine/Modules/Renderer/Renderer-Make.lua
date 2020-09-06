-- Renderer

rootDirectoryPath = "../../../"
engineDirectory = "../../"

RendererIncludeDirs = {}
RendererIncludeDirs["Nvidia"] 	    = engineDirectory .. "Vendor/Nvidia"
RendererIncludeDirs["Microsoft"]    = engineDirectory .. "Vendor/Microsoft"
RendererIncludeDirs["ImGui"] 	    = engineDirectory .. "Vendor/imgui"
RendererIncludeDirs["spdlog"] 	    = engineDirectory .. "Vendor/spdlog"
RendererIncludeDirs["rapidjson"]    = engineDirectory .. "Vendor/rapidjson"
RendererIncludeDirs["assimp"] 	    = engineDirectory .. "Vendor/assimp-3.3.1/include"
RendererIncludeDirs["Mono"] 		= engineDirectory .. "Vendor/Mono/include/mono-2.0"


project("Renderer")
    location("../Renderer")

    kind "StaticLib"
	cppdialect "C++17"
    language "C++"
	staticruntime "off"

    pchheader ("Renderer_pch.h")
    pchsource ("Source/Renderer_pch.cpp")

	targetdir (rootDirectoryPath .. "Bin/" .. outputdir .. "/%{prj.name}")
    objdir (rootDirectoryPath .. "Bin-Int/" .. outputdir .. "/%{prj.name}")
    
    files
	{
		"Source/**.cpp",
		"Source/**.h",
		"Source/**.Pixel.hlsl",
		"Source/**.Vertex.hlsl",
		"Source/**.hlsli",
    }

    includedirs
    {
		"%{RendererIncludeDirs.rapidjson}/include/",
        "%{RendererIncludeDirs.Microsoft}/DirectX12/",
        "%{RendererIncludeDirs.Microsoft}/DirectX12/WinPixEventRuntime.1.0.161208001/Include/",
        "%{RendererIncludeDirs.Microsoft}/",
		"%{RendererIncludeDirs.Nvidia}/DirectX12/",
		"%{RendererIncludeDirs.spdlog}/include/",
		"%{RendererIncludeDirs.ImGui}/",
		"%{RendererIncludeDirs.assimp}/",
		"%{RendererIncludeDirs.Mono}/",
        
        engineDirectory .. "Source/",
        "Source/"
    }
    
    
    
    -- Shaders
    filter {"system:windows"}
        defines
		{
			"IE_PLATFORM_WINDOWS"
        }
        
        filter { "files:**.pixel.hlsl" }
            shadertype "Pixel"
            shadermodel "5.0"
        
        filter { "files:**.vertex.hlsl" }
            shadertype "Vertex"
            shadermodel "5.0"
        
    -- End filter - windows
    
    
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
        "IE_IS_STANDALONE",
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
        "IE_IS_STANDALONE"
    }
-- Full Game Distribution, all engine debug tools(level editors, editor user interfaces) stripped
filter "configurations:Game-Dist"
    defines "IE_GAME_DIST"
    runtime "Release"
    optimize "on"
    symbols "on"
    defines
    {
        "IE_IS_STANDALONE"
    }