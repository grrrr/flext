make -f makefile.pd-linux &&
{ 
	echo Now install as root
	su -c "make -f makefile.pd-linux install"
}
