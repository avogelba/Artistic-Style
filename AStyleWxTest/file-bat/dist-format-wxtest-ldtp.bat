@REM formats the astylewxtest source code in the wx test directory
@echo off

set progdir=C:\Program Files\AStyle
set options=--options=..\..\AStyle\file\astyle.ini

echo Formatting AStyleWxTest
echo.

"%progdir%\AStyle"  %options%  -Q  ..\src\*.cpp
echo.
"%progdir%\AStyle"  %options%  -Q  ..\src\*.h
echo.

echo.
pause
