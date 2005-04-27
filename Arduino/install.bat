@echo off

REM Change this next line to point to the directory
REM that wiring 0003 is installed in.
SET WIRINGDIR=""

IF %WIRINGDIR%=="" GOTO ERROR

COPY wiringlite\makefile.win wiringlite\makefile
COPY wiringlite\include\makefile.win wiringlite\include\makefile
COPY wiringlite\bin\gnumake.win wiringlite\bin\gnumake
COPY pde.jar %WIRINGDIR%\lib
MKDIR %WIRINGDIR%\lib\wiringlite
XCOPY /y /s wiringlite %WIRINGDIR%\lib\wiringlite

GOTO END

:ERROR

ECHO You need to specify your wiring directory in install.bat.
PAUSE

:END
