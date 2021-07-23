git submodule update --init --recursive

mkdir "%~dp0..\OpenAL-SDK"

xcopy "%~dp0OpenAL-SDK\" "%~dp0..\OpenAL-SDK\" /y /e