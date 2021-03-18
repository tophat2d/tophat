# tophat
A 2d game engine using [rawdraw](https://github.com/cntools/rawdraw). It aims to be both lightweight and easy to use.

## issues

 - collisions seem to be a bit buggy, when you stretch your shape too much
 - images don't perform well. 55 of test.bmp are only possible at 60 fps full hd. on lower resolutions, images are ok. 

## features

- scripting with [umka](https://github.com/vtereshkov/umka-lang)
- you have your own game loop - you have the power over how the game works. not the engine
- scaling - your game looks the same no matter the resolution
- very lightweight - games are about 400 kb without assets
- collisions
- bugs (this feature is hidden, and you will get a cake as a reward for discovering it)

## umka bindings status

Bindings are currently on par with the engine. I would even say, they have more features.

## TODO

Things I want to prioritize on. They are in the order, I want to complete them, but the further into the list you are, the less accurate it is.

- collisions accounting for transformation
- image flipping
- docs
- sounds
- v0.1 release
- physics engine (some simple one)
- simple UI toolkit
- module support
- particle system
- tilemaps

## MIGHTDO

This is an experimental territory. These features aren't important to the engine, but I want to implement them one day. They are unsorted.

- doom-like 3d game
- ldtk parsing
- game jam or something
