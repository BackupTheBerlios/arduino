cd `dirname $0`

echo -n "Enter the path to the Wiring (e.g. /Applications/wiring-0003): "
read wiringdir

cp wiringlite/makefile.osx wiringlite/makefile
cp wiringlite/include/makefile.osx wiringlite/include/makefile
cp wiringlite/bin/gnumake.osx wiringlite/bin/gnumake
cp -R wiringlite $wiringdir/lib
cp pde.jar $wiringdir/Wiring.app/Contents/Resources/Java