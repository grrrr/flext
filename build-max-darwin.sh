#!/bin/sh  

. config-max-darwin.txt

make -f makefile.max-darwin &&
{ 
	if [ $INSTDIR != "" ]; then
		echo Now install as root
		sudo make -f makefile.max-darwin install
	fi
}
