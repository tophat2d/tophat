#!/bin/sh -e

# check main.um
./tophat -check

for f in tests/*.um; do
	echo CH $f
	./tophat -check -main $f
done

echo CH demos/extensions
cd demos/extensions
../../tophat -check

echo CH demos/flappy
cd ../flappy
../../tophat -check

echo CH demos/hello-world
cd ../hello-world
../../tophat -check

echo CH demos/platformer
cd ../platformer
../../tophat -check

echo CH demos/pomodoro
cd ../pomodoro
../../tophat -check

echo CH demos/space-shooter
cd ../space-shooter
../../tophat -check

echo CH demos/tetris
cd ../tetris
../../tophat -check
