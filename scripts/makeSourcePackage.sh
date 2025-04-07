#!/bin/sh

###################### PARAMETERS ####################
version="1.0"
gitpath="git@github.com:csoler/evolution-latex-plugin.git"
workdir=evolution-latex-plugin-$version
#commit="304"
######################################################

dir=`basename $PWD`

if ! test "$dir" = "evolution-latex-plugin"; then
	echo Please run this script from inside the evolution-latex-plugin/ directory.
	exit
fi

if test -d "$workdir" ;  then
	echo Please remove the $workdir directory first \(call scripts/clean.sh\).
	exit
fi
if test "$1" = "" ; then
	echo attempting to get git revision number...
	commit=`git rev-list --all --count`
else
	echo git number has been provided. Forcing update.
	commit="$1"
fi
version="$version"."$commit"

echo This is packaging evolution-plugin-latex-equations from git.
echo    git commit number: $commit

git clone $gitpath $workdir

echo Cleaning 

if ! test -d "$workdir" ; then
	echo $workdir missing. Exiting.
	exit
fi

cd $workdir
find . -name ".git" -exec \rm -rf {} \;

for i in jammy noble ; do
	echo copying changelog for $i
	cat ../scripts/changelog | sed -e s/XXXXXX/"$commit"/g | sed -e s/YYYYYY/"$i"/g > debian/changelog

	if test "$i" = "precise" ; then
		echo Distribution is precise, using specific control file.
		cp ../debian/control.precise debian/control
	elif test "$i" = "jammy" ; then
		echo Distribution is jammy, using specific control file.
		cp ../debian/control.jammy debian/control
	elif test "$i" = "noble" ; then
		echo Distribution is noble, using specific control file.
		cp ../debian/control.noble debian/control
	else
		cp ../debian/control debian/control
	fi

	# This is the key for "Cyril Soler <csoler@sourceforge.net>"
	debuild -d -S -kEFD19E9DC737CA98
done

cd ..
echo Cleaning...
\rm -rf $workdir

echo Done.

