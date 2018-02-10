@Echo off
SETLOCAL
SETLOCAL EnableDelayedExpansion

:help
ECHO ****************************************
ECHO ^
**        BEGIN WINDOWS BUILD         **
ECHO ****************************************

:: set these parameters
SET SRC_DIR=E:\src\github.com\erichiller\hues
SET OUT_EXE=go_hue.exe
:: Compiler <https://docs.microsoft.com/en-us/cpp/build/reference/cl-environment-variables>
SET CL=/FC /EHsc /W3
:: Linker <https://docs.microsoft.com/en-us/cpp/build/reference/link-environment-variables>
SET LINK=/LIBPATH:%SRC_DIR%\ ^
Ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib library\mbedTLS.lib msvcrt.lib ^
/DYNAMICBASE /NXCOMPAT /NODEFAULTLIB:library ^
/DEBUG:FULL



IF [%1]==[] (
	ECHO ****************************************
	ECHO no option selected
	GOTO help
)

IF %1 == hue GOTO build_hue
IF %1 == test_hue_http GOTO test_hue_http


:help
ECHO ****************************************
ECHO **               HELP                 **
ECHO ****************************************
ECHO **        AVAILABLE COMMANDS          **
ECHO ** ================================== **
ECHO **  >  hue                            **
ECHO **  >  test_test_hue_http             **
ECHO ****************************************
ECHO ** fin.
EXIT /B 1


:build_hue
:: Files
SET CL=%CL% ^
%SRC_DIR%\_hues.cpp ^
%SRC_DIR%\SerialPort.cpp
cl /link /OUT:%OUT_EXE%
GOTO :eof


:test_hue_http
SET CL=%CL% ^
%SRC_DIR%\_test_hue_http.cpp ^
%SRC_DIR%\SerialPort.cpp
cl /link /OUT:%OUT_EXE%
GOTO :eof

ENDLOCAL