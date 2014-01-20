#! /bin/sh
echo "starting WWW server ..."
cd bin
if [ -f shttpd -a -x shttpd ]; then
	LD_LIBRARY_PATH="$LD_LIBRARY_PATH:."
	export LD_LIBRARY_PATH
	./shttpd >> /dev/null 2>&1 &
	echo "... started on port 8000"
	echo
	echo "Now use your browser and connect to"
	echo "        http://localhost:8000"
	echo "to read the online documentation"
else
	echo "... can't do that - You have to compile the sources first!"
	echo "    (go to the src/ directory and start 'autoconf.sh')"
fi
