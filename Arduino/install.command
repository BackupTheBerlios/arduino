#!/bin/tcsh

cd `dirname $0`

echo "What directory is Wiring in?"
set wiringdir = $<
echo Installing to $wiringdir

cp wiringlite/makefile.osx wiringlite/makefile
cp wiringlite/include/makefile.osx wiringlite/include/makefile
cp wiringlite/bin/gnumake.osx wiringlite/bin/gnumake
cp -R wiringlite ${wiringdir}/lib
rm ${wiringdir}/lib/wiringlite/tmp/dep/README.txt
cp pde.jar ${wiringdir}/Wiring.app/Contents/Resources/Java
