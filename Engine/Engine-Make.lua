-- Engine
-- Engine project is a container for the source code. It does not build anything.
-- Useful as a development project to keep track of or add new files.

rootDirPath = "../"
projectName = "Engine/"

project (projectName)
	location (rootDirPath .. projectName)
	kind ("SharedItems")
	language ("C++")
	cppdialect ("C++17")

