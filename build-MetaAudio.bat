call cmake -B "build" -A Win32

call powershell -Command "(gc build\MetaAudio.vcxproj) -replace 'Label=\"PropertySheets\">', 'Label=\"PropertySheets\" ><Import Project=\"..\global.props\" /><Import Project=\"..\MetaAudio.props\" />' | Out-File build\MetaAudio.vcxproj"

for /f "usebackq tokens=*" %%i in (`vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
  set InstallDir=%%i
)

if exist "%InstallDir%\Common7\Tools\vsdevcmd.bat" (

    "%InstallDir%\Common7\Tools\vsdevcmd.bat" -arch=x86
    
    MSBuild.exe "build\MetaAudio.sln" /t:MetaAudio /p:Configuration=Release /p:Platform="Win32"
)