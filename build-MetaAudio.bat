mkdir "%temp%\OpenAL-SDK"

xcopy "%~dp0OpenAL-SDK\" "%temp%\OpenAL-SDK\" /y /e

call cmake -B "build" -A Win32 -D "OPENAL_INCLUDE_DIR=%temp%\OpenAL-SDK\include" -D "OPENAL_LIBRARY=%temp%\OpenAL-SDK\libs\Win32\OpenAL32.lib" -D "SNDFILE_LIBRARY=%~dp0externals/libsndfile/lib/sndfile.lib" -D "SNDFILE_INCLUDE_DIR=%~dp0externals/libsndfile/include"

call powershell -Command "(gc build\MetaAudio.vcxproj) -replace 'Label=\"PropertySheets\">', 'Label=\"PropertySheets\" ><Import Project=\"..\global.props\" /><Import Project=\"..\MetaAudio.props\" />' | Out-File build\MetaAudio.vcxproj"

for /f "usebackq tokens=*" %%i in (`vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
  set InstallDir=%%i
)

if exist "%InstallDir%\Common7\Tools\vsdevcmd.bat" (

    "%InstallDir%\Common7\Tools\vsdevcmd.bat" -arch=x86
    
    MSBuild.exe "build\MetaAudio.sln" /t:MetaAudio /p:Configuration=Release /p:Platform="Win32"
    MSBuild.exe "build\MetaAudio.sln" /t:metahook /p:Configuration=Release /p:Platform="Win32"
)

rmdir /s /q "%temp%\OpenAL-SDK"