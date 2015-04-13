@echo off
echo HTTP/1.0 200 Okay
echo Content-type: text/plain
echo.
echo Test-CGI okay
echo.
echo Parameters:
echo -----------
echo Command Line Parameter #0: %0
echo Command Line Parameter #1: %1
echo Command Line Parameter #2: %2
echo Command Line Parameter #3: %3
echo Command Line Parameter #4: %4
echo Command Line Parameter #5: %5
echo Command Line Parameter #6: %6
echo Command Line Parameter #7: %7
echo Command Line Parameter #8: %8
echo Command Line Parameter #9: %9
echo.
rem sleep 20
echo Environment:
echo ------------
set
echo.
echo Done.
