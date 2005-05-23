@echo off

REM This installs the Windows 2000 versions of the makefiles.

REM Change this next line to point to the directory
REM that wiring 0003 is installed in, without a trailing
REM slash, e.g. "c:\wiring-0003"
SET WIRINGDIR=""

IF %WIRINGDIR%=="" GOTO ERROR

COPY wiringlite\makefile.w2k wiringlite\makefile
COPY wiringlite\include\makefile.w2k wiringlite\include\makefile
COPY wiringlite\bin\gnumake.win wiringlite\bin\gnumake.exe
COPY pde.jar %WIRINGDIR%\lib
MKDIR %WIRINGDIR%\lib\wiringlite
XCOPY /y /s wiringlite %WIRINGDIR%\lib\wiringlite
DEL %WIRINGDIR%\lib\wiringlite\tmp\dep\README.txt

GOTO END

:ERROR

ECHO You need to specify your wiring directory in install.bat.
PAUSE

:END
