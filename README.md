# tophat engine
A 2d game engine using [rawdraw](https://github.com/cntools/rawdraw). It aims to be both lightweight and easy to use.

## features

- scripting with [umka](https://github.com/vtereshkov/umka-lang)
- you have your own game loop - you have the power over how the game works. not the engine
- does things the simple way - for example, tophat has no bloated system for animations. you can make them in just one line.
- scaling - your game looks the same no matter the resolution
- very lightweight - the engine is only about 750 kb in size
- polygon to polygon collisions

## TODO

Things I want to prioritize on. They are in the order, I want to complete them, but the further into the list you are, the less accurate it is.

- more safety
- convert coordinates and rotation to real
- add better return values from collisions
- notes on entities
- minitutorials - how to structure, animations, raycasts, wasd movement etc.
- simple UI toolkit
- v0.1 release
- web build
- physics engine (some simple one)
- module support
- particle system
- tilemaps

## MIGHTDO

This is an experimental territory. These features aren't important to the engine, but I want to implement them one day. They are unsorted.

- doom-like 3d game
- ldtk parsing
- game jam or something

## special thanks

Here are people, who made a library, that is really important to tophat, or helped me with making tophat (or both).

- [vtereshkov](https://github.com/vtereshkov) - for creating the beatiful language, that is [umka](https://github.com/vtereshkov/umka-lang). tophat wouldn't be the same without umka. big thanks and have luck with your project
- [cnlohr](https://github.com/cnlohr) - for creating rawdraw, although I eventually had to fork it :]
- [avivbeeri](https://github.com/avivbeeri) - for creating dome engine (my inspiration) and helping me in the early days of the engine
- [prokoprandacek](https://github.com/prokoprandacek) - for his contributions
