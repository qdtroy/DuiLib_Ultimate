@Echo Off
SET BAK_PATH=%PATH%
SET PLATFORM=x86
SET CMAKE_PATH=%CD%\cmake-3.4.3-win32-x86\bin;
SET DEVENV_PATH=C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE;
SET BIN_PATH=%CD%\..\Bin;
SET XCOPY=xcopy /E /Y /I /Q
SET PATH=%PATH%;%CMAKE_PATH%;%DEVENV_PATH%;%BIN_PATH%

CD cef_binary
CMake .

:quit
SET XCOPY=
SET PATH=%BAK_PATH%
