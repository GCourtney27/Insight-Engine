
projectName = "Shaders"

project (projectName)
    location ("%{wks.location}/Engine/" .. projectName)
    kind ("Utility")
    targetname ("%{prj.name}")

    targetdir (ieGetBuildFolder() .. "/" .. projectName)
    objdir (ieGetBuildIntFolder() .. "/" .. projectName)

    files
    {
        "Shaders-Make.lua",

        "HLSL/Common/**.hlsl*",
        "HLSL/DeferredRendering/**.hlsl*",
        "HLSL/ForwardRendering/**.hlsl*",
        "HLSL/Shared/**.hlsl*",
        "HLSL/Public/**.hlsl*",
        "HLSL/Private/**.hlsl*"
    }                     

    postbuildcommands
    {
        "call HLSL/Compile-RT-Shaders.bat $(TargetDir)",
    }
    
    filter { "files:**.vs.hlsl" }
        shadertype "Vertex"
        shadermodel "5.1"

    filter { "files:**.ps.hlsl" }
        shadertype "Pixel"
        shadermodel "5.1"

    filter { "files:**.cs.hlsl" }
        shadertype "Compute"
        shadermodel "5.1"
