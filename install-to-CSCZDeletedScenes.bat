echo off

if not exist "%~dp0build\externals\metahook\Release\metahook.exe" goto fail_nobuild
if not exist "%~dp0build\Release\MetaAudio.dll" goto fail_nobuild

set LauncherExe=metahook.exe
set LauncherMod=czeror

for /f "delims=" %%a in ('"%~dp0SteamAppsLocation/SteamAppsLocation" 100 InstallDir') do set GameDir=%%a

if "%GameDir%"=="" goto fail

echo -----------------------------------------------------

echo Copying files...

if not exist "%GameDir%\%LauncherExe%" copy "%~dp0build\externals\metahook\Release\metahook.exe" "%GameDir%\%LauncherExe%" /y
copy "%~dp0externals\libsndfile\bin\sndfile.dll" "%GameDir%\" /y
md "%GameDir%\%LauncherMod%\metahook\plugins"
copy "%~dp0build\Release\MetaAudio.dll" "%GameDir%\%LauncherMod%\metahook\plugins" /y

powershell $shell = New-Object -ComObject WScript.Shell;$shortcut = $shell.CreateShortcut(\"MetaHook for CSCZDeletedScenes.lnk\");$shortcut.TargetPath = \"%GameDir%\%LauncherExe%\";$shortcut.WorkingDirectory = \"%GameDir%\";$shortcut.Arguments = \"-steam -insecure -game %LauncherMod%\";$shortcut.Save();

notepad "%GameDir%\%LauncherMod%\metahook\configs\plugins.lst"

echo -----------------------------------------------------

echo Done
echo Make sure that "MetaAudio.dll" is in the metahook plugin load list (plugins.lst) and it's on top of any other plugins.
echo Launch game from "MetaHook for CSCZDeletedScenes" shortcut.
pause
exit

:fail

echo Failed to locate GameInstallDir of Counter-Strike : Condition Zero - Deleted Scenes, please make sure Steam is running and you have Counter-Strike : Condition Zero - Deleted Scenes installed correctly.
pause
exit

:fail_nobuild

echo Compiled binaries not found ! You have to download compiled zip from github release page or compile the sources by yourself before installing !!!
pause
exit