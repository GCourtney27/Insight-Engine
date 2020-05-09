REM Build assimp
cd Engine\Vendor\assimp-3.3.1
mkdir build
cd build
echo *****************************************
echo Creating Visual Studio 2019 project files
echo *****************************************
%~dp0Vendor\CMake\bin\cmake.exe %~dp0Engine\vendor\assimp-3.3.1
echo *****************************************
echo Compiling assimp source code
echo *****************************************
%~dp0Project_Build_Scripts\Compile_Assimp.bat

REM Build DirectX Tool Kit (TK)
cd ..
echo *****************************************
echo Compiling DirectX Tool Kit source code
echo *****************************************
%~dp0Project_Build_Scripts\Compile_DirectXTK.bat

PAUSE