
ECHO Compiling ray tracing shaders.

cd /D "%~dp0"
mkdir BinTemp

@REM Compile the shaders.
for /r %%f in (RayTracing\*.lib.hlsl) do (
    ..\..\Tools\DxcAPI\bin\x64\dxc.exe -Qembed_debug -T lib_6_3 -Fo BinTemp/%%~nf.cso -Zi %%f
)

@REM Copy the files to the specified command line output directory.
xcopy /s/e/q/y "BinTemp\*.lib.cso" "%1"

@REM Delete the intermediate output directory.
rmdir /s/q "BinTemp\"

ECHO Finished compiling ray tracing shaders