REM Build assimp
cd Engine\Vendor\assimp-3.3.1
mkdir build
cd build
echo ***************************************** & echo.
echo Creating Visual Studio 2019 project files & echo.
echo ***************************************** & echo.
%~dp0Vendor\CMake\bin\cmake.exe %~dp0Engine\vendor\assimp-3.3.1
echo ***************************************** & echo.
echo Compiling assimp source code & echo.
echo ***************************************** & echo.
%~dp0Project_Build_Scripts\Compile_Assimp.bat

REM Build DirectX Tool Kit (TK)
cd ..
echo ***************************************** & echo.
echo Compiling DirectX 12 Tool Kit source code & echo.
echo ***************************************** & echo.
%~dp0Project_Build_Scripts\Compile_DirectXTK.bat

PAUSE