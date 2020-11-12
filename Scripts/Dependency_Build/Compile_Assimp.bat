echo Config: Debug
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\Common7\IDE\devenv" ..\..\Engine\Third_Party\assimp-3.3.1\build\Assimp.sln /build Debug /project ..\..\Engine\Third_Party\assimp-3.3.1\build\ALL_BUILD.vcxproj /projectconfig Debug

echo Config: Release
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\Common7\IDE\devenv" ..\..\Engine\Third_Party\assimp-3.3.1\build\Assimp.sln /build Release /project ..\..\Engine\Third_Party\assimp-3.3.1\build\ALL_BUILD.vcxproj /projectconfig Release
