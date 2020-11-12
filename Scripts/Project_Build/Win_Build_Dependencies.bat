REM Build assimp
cd ..\Engine\Third_Party\assimp-3.3.1
mkdir build
cd build
echo ***************************************** & echo.
echo Creating Visual Studio 2019 project files & echo.
echo ***************************************** & echo.
%~dp0..\..\Third_Party\CMake\bin\cmake.exe %~dp0Engine\vendor\assimp-3.3.1
echo ***************************************** & echo.
echo Compiling assimp source code & echo.
echo ***************************************** & echo.
call %~dp0..\Dependency_Build\Compile_Assimp.bat

cd ..\..\..\..\

REM Build DirectX 12 Tool Kit (TK)
echo ***************************************** & echo.
echo Compiling DirectX 12 Tool Kit source code & echo.
echo ***************************************** & echo.
call %~dp0..\Dependency_Build\Compile_DirectX_12_TK.bat

REM Build DirectX 12 Texture Tools
echo ***************************************** & echo.
echo Compiling DirectX 12 Texture Tools source code & echo.
echo ***************************************** & echo.
call %~dp0..\Dependency_Build\Compile_DirectX_12_Tex.bat

REM Build DirectX 11 Tool Kit (TK)
echo ***************************************** & echo.
echo Compiling DirectX 11 Tool Kit source code & echo.
echo ***************************************** & echo.
call %~dp0..\Dependency_Build\Compile_DirectX_11_TK.bat

PAUSE