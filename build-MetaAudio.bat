mkdir "%temp%\OpenAL-SDK"

xcopy "%~dp0OpenAL-SDK\" "%temp%\OpenAL-SDK\" /y /e

call cmake -B "build" -A Win32 -D "OPENAL_INCLUDE_DIR=%temp%\OpenAL-SDK\include" -D "OPENAL_LIBRARY=%temp%\OpenAL-SDK\libs\Win32\OpenAL32.lib"

call powershell -Command "(gc build\MetaAudio.vcxproj) -replace 'Label=\"PropertySheets\">', 'Label=\"PropertySheets\" ><Import Project=\"..\global.props\" /><Import Project=\"..\MetaAudio.props\" />' | Out-File build\MetaAudio.vcxproj"

call powershell -Command "(gc build\externals\alure\alure2.vcxproj) -replace 'Label=\"PropertySheets\">', 'Label=\"PropertySheets\" ><Import Project=\"..\..\..\global.props\" /><Import Project=\"..\..\..\alure2.props\" />' | Out-File build\externals\alure\alure2.vcxproj"

call powershell -Command "(gc build\externals\openal-soft\OpenAL.vcxproj) -replace 'Label=\"PropertySheets\">', 'Label=\"PropertySheets\" ><Import Project=\"..\..\..\global.props\" /><Import Project=\"..\..\..\OpenAL.props\" />' | Out-File build\externals\openal-soft\OpenAL.vcxproj"

call powershell -Command "(gc build\externals\metahook\metahook.vcxproj) -replace 'Label=\"PropertySheets\">', 'Label=\"PropertySheets\" ><Import Project=\"..\..\..\global.props\" /><Import Project=\"..\..\..\metahook.props\" />' | Out-File build\externals\metahook\metahook.vcxproj"

for /f "usebackq tokens=*" %%i in (`vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
  set InstallDir=%%i
)

if exist "%InstallDir%\Common7\Tools\vsdevcmd.bat" (

    "%InstallDir%\Common7\Tools\vsdevcmd.bat" -arch=x86
    
    MSBuild.exe "build\MetaAudio.sln" /t:MetaAudio /p:Configuration=Release /p:Platform="Win32"
    MSBuild.exe "build\MetaAudio.sln" /t:metahook /p:Configuration=Release /p:Platform="Win32"
)

rmdir /s /q "%temp%\OpenAL-SDK"