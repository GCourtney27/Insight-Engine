## Insight Engine
A full game engine made for my 2019 Capstone project. It is written in C/C++ with DirectX 11 being the driver for the graphics.

**World Editor**
* Quickly update the transforms and components of your objects in real-time without having to close the engine and make adjustments. ImGui being used to provide quick window docking display information for Game Objects

**Physically-Based Rendering with Image-Based Lighting**
* Insight uses a full PBR pipeline for materials. Albedo, Normal, Roughness, Metallic, and Ambient Occlusion maps are all put to use to produce very realistic Surfaces. Specular Image-Based Lighting (IBL) is also used to ground the objects in the scene

**Lua Scripting**
* Lua allows for quick modification to a Game Object's behavior in the scene simply add a new Lua Script Component and get your objects interacting with each other.

**JSON File IO**
* Save and load your scenes. In the background, JSON is being used to store all Game Objects and their corresponding components

**Platforms**
* Windows PCDesktop (x64)
* Visual Studio 2019/*2017

**Getting Started**
1) After cloning the repo into a directory, run the "Build_Dependencies.bat" file to build all dependencies.
2) Run "Generate_Projects_VS2019.bat" to generate the Visual studio project files. Note: Visual Studio 2017 is supported, however, you must change the Visual Studio location to Visual Studio 2017 in "Generate_Projects_VS2019.bat" and  "Project_Build_Scripts/Build_Assimp.bat" as this script searches for your local Visual Studio command line executable to build the respective assimp binaries.
3) Open the generated Visual Studio solution and enjoy. You dont need to worry about copying dependency binaries to the output directory, post build steps will achieve this for you. 
