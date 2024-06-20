#!/bin/sh -e

# check main.um
./tophat -check

for f in tests/*.um; do
	./tophat -check -main $f
done

cd demos/extensions
../../tophat -check

cd ../flappy
../../tophat -check

cd ../hello-world
../../tophat -check

cd ../platformer
../../tophat -check

cd ../pomodoro
../../tophat -check

cd ../space-shooter
../../tophat -check

cd ../tetris
../../tophat -check
