# tophat
A 2d game engine using [rawdraw](https://github.com/cntools/rawdraw). It still is very work in progress.

## features

- basic entity
- polygon to polygon collisions
- bugs (this feature is hidden, and you will get a cake as a reward for discovering it)

## currently working on

- umka scripting
- bitmap parser

## umka bindings status

- rawdraw
	working: CNFGSetup, CNFGBgColor, CNFGColor, CNFGClearFrame, CNFGSwapBuffers
	issues: CNFGGetDimensions - segfaults in the umkavm
	notes: i do not play on making full rawdraw bindings. these will only support stuff such as making window. rest will be done through the engine.
- engine - nearly nothing

## TODO

things i want to prioritize on. they are in the order, i want to complete them, but the further into the list you are, the less accurate it is.

- better input handling
- image operation (scaling, flipping, rotating)
- sounds
- build tool
- v0.1 release
- project file
- scalable font
- simple ui toolkit
- module support

## MIGHTDO

this is experimental territorry. these features aren't important to the engine, but i want to implement them one day. they are unsorted.

- doom-like 3d game
- particle system
- tilemaps
- physics engine (some simple one)
- ldtk parsing
- game jam or something
