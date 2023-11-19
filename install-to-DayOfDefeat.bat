echo off

if not exist "%~dp0build\Release\metahook.exe" goto fail_nobuild
if not exist "%~dp0build\Release\MetaAudio.dll" goto fail_nobuild

set LauncherExe=metahook.exe
set LauncherMod=dod

for /f "delims=" %%a in ('"%~dp0SteamAppsLocation/SteamAppsLocation" 30 InstallDir') do set GameDir=%%a

if "%GameDir%"=="" goto fail

echo -----------------------------------------------------

echo Copying files...

if not exist "%GameDir%\%LauncherExe%" copy "%~dp0build\Release\metahook.exe" "%GameDir%\%LauncherExe%" /y
copy "%~dp0externals\libsndfile\bin\sndfile.dll" "%GameDir%\" /y
md "%GameDir%\%LauncherMod%\metahook\plugins"
copy "%~dp0build\Release\MetaAudio.dll" "%GameDir%\%LauncherMod%\metahook\plugins" /y

notepad "%GameDir%\%LauncherMod%\metahook\configs\plugins.lst"

powershell $shell = New-Object -ComObject WScript.Shell;$shortcut = $shell.CreateShortcut(\"MetaHook for DayOfDefeat.lnk\");$shortcut.TargetPath = \"%GameDir%\%LauncherExe%\";$shortcut.WorkingDirectory = \"%GameDir%\";$shortcut.Arguments = \"-steam -insecure -game %LauncherMod%\";$shortcut.Save();

echo -----------------------------------------------------

echo Done
echo Make sure that "MetaAudio.dll" is in the metahook plugin load list (plugins.lst) and it's on top of any other plugins.
echo Launch game from "MetaHook for DayOfDefeat" shortcut.
pause
exit

:fail

echo Failed to locate GameInstallDir of Day of Defeat, please make sure Steam is running and you have Day of Defeat installed correctly.
pause
exit

:fail_nobuild

echo Compiled binaries not found ! You have to download compiled zip from github release page or compile the sources by yourself before installing !!!
pause
exit