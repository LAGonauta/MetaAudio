call cmake -B "build" -A Win32 -D "OPENAL_INCLUDE_DIR=%~dp0..\OpenAL-SDK\include" -D "OPENAL_LIBRARY=%~dp0..\OpenAL-SDK\libs\Win32\OpenAL32.lib"

for /f "usebackq tokens=*" %%i in (`vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
  set InstallDir=%%i
)

if exist "%InstallDir%\Common7\Tools\vsdevcmd.bat" (

    "%InstallDir%\Common7\Tools\vsdevcmd.bat" -arch=x86
    
    MSBuild.exe "build\MetaAudio.sln" /t:MetaAudio /p:Configuration=Release /p:Platform="Win32"
)