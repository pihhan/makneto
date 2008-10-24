Zprovozneni teto verze makneta:

Je treba nejprve do tohoto adresare stahnout iris, do adresare jmenem iris.

$ svn co https://delta.affinix.com/svn/trunk/iris iris

sehnat si qconf z http://delta.affinix.com/qconf/

$ svn checkout https://delta.affinix.com/svn/trunk/qconf qconf
Vyrobit qconf
$ cd qconf
$ ./configure
$ make
$ cd ..

pote opatchovat iris na podporu whiteboardingu,
zkompilovat iris tak, aby knihovny byly v iris/lib
$ cd iris
$ patch -p0 -i ../patches/whiteboard.patch
$ ../qconf/qconf
$ ./configure --debug
$ make

az nyni je mozne provozovat makneto
$ cd makneto
$ cmake .
$ make

A melo by to fungovat
