-- Engine
-- Engine project is a container for the source code. It does not build anything.

projectName = "Engine"

project (projectName)
	location (rootDirPath .. projectName)
	kind ("SharedItems")
	language ("C++")
	cppdialect ("C++17")
	