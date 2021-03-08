# tophat
A 2d game engine using [rawdraw](https://github.com/cntools/rawdraw). It still is a very work in progress.

## features

- basic entity
- polygon to polygon collisions
- bugs (this feature is hidden, and you will get a cake as a reward for discovering it)

## currently working on

- umka scripting
- bitmap parser

## umka bindings status

- rawdraw
  - working: CNFGSetup, CNFGBgColor, CNFGColor, CNFGClearFrame, CNFGSwapBuffers, text drawing
  - notes: I do not play on making full rawdraw bindings. these will only support stuff such as making a window. rest will be done through the engine.
- engine
  - working: rectangles, polygons, entities, collisions

## TODO

Things I want to prioritize on. They are in the order, I want to complete them, but the further into the list you are, the less accurate it is.

- fix just pressed function
- image operation (scaling, flipping, rotating)
- sounds
- build tool
- v0.1 release
- project file
- simple UI toolkit
- module support

## MIGHTDO

This is an experimental territory. These features aren't important to the engine, but I want to implement them one day. They are unsorted.

- doom-like 3d game
- particle system
- tilemaps
- physics engine (some simple one)
- ldtk parsing
- game jam or something
