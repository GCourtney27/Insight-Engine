REM Build assimp
cd Engine\vendor\assimp-3.3.1
mkdir build
cd build
echo Creating Visual Studio 2019 project files
%~dp0vendor\bin\CMake\bin\cmake.exe %~dp0Engine\vendor\assimp-3.3.1
echo Compiling assimp source code
%~dp0Project_Build_Scripts\CompileAssimp.bat

PAUSE