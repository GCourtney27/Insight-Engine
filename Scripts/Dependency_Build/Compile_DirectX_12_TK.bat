echo Config: Debug
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\Common7\IDE\devenv" ..\..\Engine_Source\Third_Party\Microsoft\DirectX12\TK\DirectXTK_Desktop_2019_Win10.sln /build "Debug|x64" /project ..\..\Engine_Source\Third_Party\Microsoft\DirectX12\TK\DirectXTK_Desktop_2019_Win10.vcxproj /projectconfig "Debug|x64"

echo Config: Release
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\Common7\IDE\devenv" ..\..\Engine_Source\Third_Party\Microsoft\DirectX12\TK\DirectXTK_Desktop_2019_Win10.sln /build "Release|x64" /project ..\..\Engine_Source\Third_Party\Microsoft\DirectX12\TK\DirectXTK_Desktop_2019_Win10.vcxproj /projectconfig "Release|x64"
