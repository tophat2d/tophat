# tophat
A 2d game engine using [rawdraw](https://github.com/cntools/rawdraw). It aims to be both lightweight and easy to use.

## features

- scripting with [umka](https://github.com/vtereshkov/umka-lang)
- you have your own game loop - you have the power over how the game works. not the engine
- scaling - your game looks the same no matter the resolution
- very lightweight - games are about 400 kb without assets
- collisions
- bugs (this feature is hidden, and you will get a cake as a reward for discovering it)

## umka bindings status

Umka binding currently lack only image support, but since images still aren't drawn efficiently enough to be used, I don't plan on making bindings for them.

## TODO

Things I want to prioritize on. They are in the order, I want to complete them, but the further into the list you are, the less accurate it is.

- faster images
- image transforming
- docs
- sounds
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
