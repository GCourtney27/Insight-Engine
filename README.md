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


*This software falls under the MIT license

Copyright (c) 2019 Garrett Courtney

If you use it please give credit
