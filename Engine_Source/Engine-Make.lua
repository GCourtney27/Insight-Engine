-- Engine_Source
-- Engine_Source project is a container for the source code. It does not build anything.

project ("Engine_Source")
	location (rootDirPath .. "Engine_Source")
	kind "SharedItems"
	language "C++"
	cppdialect "C++17"
	
	-- files
	-- {
	-- 	-- This Project's Make File
	-- 	"Engine-Make.lua",

	-- 	"%{engineIncludeDirs.Engine_Source}/Third_Party/Vendor_Build.cpp",
	-- 	"%{engineIncludeDirs.Engine_Source}/Source/**.cpp",
	-- 	"%{engineIncludeDirs.Engine_Source}/Source/**.h",
	-- 	"%{engineIncludeDirs.Engine_Source}/Source/**.vertex.hlsl",
	-- 	"%{engineIncludeDirs.Engine_Source}/Source/**.pixel.hlsl",
	-- 	"%{engineIncludeDirs.Engine_Source}/Source/**.compute.hlsl",
	-- }