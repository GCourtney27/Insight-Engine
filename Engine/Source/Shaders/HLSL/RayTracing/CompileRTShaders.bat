@echo off

@REM Change to the specified output directory from command line args.
cd /D "%~dp0"
mkdir Binaries

@REM Compile the shaders.
for /r %%f in (*.rtlib.hlsl) do (
    ..\..\..\..\..\Tools\DxCompiler\dxc.exe -Qembed_debug -T lib_6_3 -Fo Binaries/%%~nf.cso -Zi %%f
)

@REM Copy the files to the specified command line output directory.
xcopy /s/e/q "Binaries\*.rtlib.cso" "%1"

@REM Delete the intermediate output directory.
rmdir /s/q "Binaries\"
