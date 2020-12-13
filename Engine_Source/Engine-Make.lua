-- Engine_Source
-- Engine_Source project is a container for the source code. It does not build anything.

projectName = "Engine_Source"

project (projectName)
	location (rootDirPath .. projectName)
	kind ("SharedItems")
	language ("C++")
	cppdialect ("C++17")
	