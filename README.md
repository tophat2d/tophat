# tophat

Tophat is a 2d game engine focused on minimalism and not taking power away from the user. Although it is made in c, you can make games using [umka](https://github.com/vtereshkov/umka-lang) a language inspired by go, that makes your gamedev easier. It currently supports linux and windows with web, android and raspberry pi support on the way.

## features

- easy to use
- tilemaps
- very lightweight
- polygon to polygon collisions
- minimalist ui framework
- linear interpolation similar to godot's tweens
- raycasts (also similar to godot)
- automatic scaling - your games look the same no matter the resolution

## getting started

If you want to look at some examples, they can be find in the `examples` folder. If you decide to start working with the engine, I recommend reading the [getting started](https://github.com/marekmaskarinec/tophat/wiki/Getting-started) article, that covers installing the engine and making a new project. Then you can continue by reading the [docs](https://github.com/marekmaskarinec/tophat/wiki/Umka-libraries).

## games made with tophat

Aside from examples, I have made a jam game called [RAIDER](https://maskarinec.itch.io/RAIDER). If you know/have made a game in tophat, don't hesitate and make a pr to add it here.

## build instructions

### deps:
  - compiler - gcc and clang are tested to work. I had some problems with tcc.
  - libGL headers
  - libX11 headers for linux build
 
...

- `make deps` - downloads all submodules and libraries and builds umka
- `make run` - to build without optimizations and embedding + run
- `make install` - build and install, so you can test on existing projects
- `make build` - release build
- `make windows` - build for windows
- `make package` - creates a package, that can be installed using linux shell tool
- `make win-package` - same as above, but is makes a windows package


## contributing

I'm open to contributions, although the c code is kind of a mess right now. I plan on adding more comments and removing unnecesarry code.

## todo

- tranparency on images
- navigation on tilemaps
- better return values from raycasts
- `move_and_slide`-like function
- particle system
- convert backend to reals and allow for pixel imperfect movement
- physics engine
- embedding of user files
- web build
- auto tile

## special thanks

Here are people, who made a library, that is really important to tophat, or helped me with making tophat (or both).

- [vtereshkov](https://github.com/vtereshkov) - for creating [umka](https://github.com/vtereshkov/umka-lang).
- [cnlohr](https://github.com/cnlohr) - for creating rawdraw
- [avivbeeri](https://github.com/avivbeeri) - for creating dome engine (my inspiration) and helping me in the early days of the engine
- [prokoprandacek](https://github.com/prokoprandacek) - for his contributions
