@echo off

for /r %%f in (*.rtlib.hlsl) do (
    ..\..\..\..\..\Tools\DxCompiler\dxc.exe -T lib_6_3 -Fo %1/%%~nf.cso -Zi %%f
)