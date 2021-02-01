@echo off

echo *****************************************
echo Creating assimp VS2019 project
echo *****************************************
cd Engine\ThirdParty\assimp-5.0.1
mkdir build
cd build
%~dp0Vendor\CMake\bin\cmake.exe %~dp0Engine\ThirdParty\assimp-5.0.1

echo *****************************************
echo Compiling assimp source code
echo *****************************************
call %~dp0Scripts\DependencyBuild\Compile_Assimp_5.0.1.bat


echo *****************************************
echo Compiling DirectX 12 Tool Kit source code
echo *****************************************
call %~dp0Scripts\DependencyBuild\Compile_DirectX_12_TK.bat


echo *****************************************
echo Compiling DirectX 11 Tool Kit source code
echo *****************************************
call %~dp0Scripts\DependencyBuild\Compile_DirectX_11_TK.bat

PAUSE