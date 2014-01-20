@echo off
echo starting WWW server ...
cd bin
if not exist shttpd.exe goto x_compile
start /min shttpd
if errorlevel 1 goto x_error
goto x_okay

:x_okay
cd ..
echo ... started on port 80
echo.
echo Now use your browser and connect to
echo         http://localhost
echo to read the online documentation.
goto x_end

:x_error
cd ..
echo ... could not start WWW server on port 80!
goto x_end

:x_compile
cd ..
echo ... can't do that - You have to compile the sources first!
echo     (go to the src\ directory and start 'autoconf')
goto x_end

:x_end
