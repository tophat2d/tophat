# tophat engine
A 2d game engine using [rawdraw](https://github.com/cntools/rawdraw) and allows scripting with [umka](https://github.com/vtereshkov/umka-lang). It aims to give as much power as possible to the user, while still being comfortable to use. To get started, read [this](https://github.com/marekmaskarinec/tophat/wiki/getting-started) article. See docs [here](https://github.com/marekmaskarinec/tophat/wiki/Umka-libraries). Examples can be found in the examples folder.

## features

- scripting with [umka](https://github.com/vtereshkov/umka-lang)
- you have your own game loop - you have the power over how the game works. not the engine
- does things the simple way
- scaling - your game looks the same no matter the resolution
- very lightweight - the engine is only about 750 kb in size
- great cli tool for linux

## TODO

- v0.1 release
- file embedding
- tilemaps
- csv parser
- particle system
- physics engine (some simple one)
- `move_and_slide`-like function
- add better return values from collisions
- web build

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
