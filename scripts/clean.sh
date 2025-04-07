#!/bin/sh

dir=`basename $PWD`

if ! test "$dir" = "LibYlm" ;  then
	echo Please run this script from top directory
	exit
fi

#cd src && make distclean
#\rm -rf src/bin src/bin.debug src/lib src/lib.debug
#\rm -rf src/*/.obj
#\rm -rf src/*/.obj.debug
\rm evolution-plugin-latex-equations_*
\rm -rf evolution-plugin-latex-equations-1.0

