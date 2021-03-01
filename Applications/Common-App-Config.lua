-- Common App Settings/Configurations
monoInstallDir = "C:/Program Files/Mono/"

commonPostBuildCommands = {}
commonPostBuildCommands["debugContentDir"] = "IF NOT EXIST $(TargetDir)Content mklink /D $(TargetDir)Content %{wks.location}Engine\\Content"
commonPostBuildCommands["releaseContentDir"] = "{COPY} %{wks.location}Engine/Content %{cfg.targetdir}/Content"
commonPostBuildCommands["debugShaderDir"] = "IF NOT EXIST $(TargetDir)Shaders mklink /D $(TargetDir)Shaders %{wks.location}Engine\\Shaders"
commonPostBuildCommands["releaseShaderDir"] = "{COPY} %{wks.location}Engine/Shaders %{cfg.targetdir}/Shaders"
commonPostBuildCommands["dxildll"] = "{COPY} %{win32AppIncludeDirs.DxcAPI}/bin/x64/dxil.dll %{cfg.targetdir}"
commonPostBuildCommands["dxcompilerdll"] = "{COPY} %{win32AppIncludeDirs.DxcAPI}/bin/x64/dxcompiler.dll %{cfg.targetdir}"
commonPostBuildCommands["PIXRuntimeUWPdll"] = "{COPY} %{uwpAppIncludeDirs.Engine_ThirdParty}/Microsoft/WinPixEventRuntime/bin/x64/WinPixEventRuntime_UAP.dll %{cfg.targetdir}"
commonPostBuildCommands["PIXRuntimeWin32dll"] = "{COPY} %{win32AppIncludeDirs.Engine_ThirdParty}/Microsoft/WinPixEventRuntime/bin/x64/WinPixEventRuntime.dll %{cfg.targetdir}"
commonPostBuildCommands["assimpdll_debug"] = "{COPY} %{win32AppIncludeDirs.Engine_ThirdParty}/assimp-5.0.1/build/code/Debug/assimp-vc142-mtd.dll %{cfg.targetdir}"
commonPostBuildCommands["assimpdll_release"] = "{COPY} %{win32AppIncludeDirs.Engine_ThirdParty}/assimp-5.0.1/build/code/Release/assimp-vc142-mt.dll %{cfg.targetdir}"
commonPostBuildCommands["monodll"] = "{COPY} \"".. monoInstallDir .."/bin/mono-2.0-sgen.dll\" %{cfg.targetdir}"
