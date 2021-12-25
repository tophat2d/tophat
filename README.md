# tophat

Tophat is a friendly 2d game engine for the [Umka](https://github.com/vtereshkov/umka-lang) programming language.
Visit the [homepage](https://marekmaskarinec.github.io/tophat.html) for more info.

## features

- easy to use
- very lightweight
- minimalist ui framework
- automatic scaling - your games look the same no matter the resolution
- great cli tool for linux
- particles
- tilemaps
- lighting system
- collision detection
- font support

## getting started

If you just want to see, how a game made using tophat looks like, you can find some in the `examples` folder.
Standard library documentation can be found [here](https://marekmaskarinec.github.io/thdocs/index.html).

## todo

See the [todo list](./todo.txt).

## build instructions

### deps:
  - compiler - `gcc`, `tcc` and `clang` should work. I didn't test any others.
  - libGL headers
  - libX11 headers for linux build
 
!!! Make sure that you cloned this repository with the `--recursive` flag !!!  
In case you didn't, run this: `git submodule init && git submodule update`  
  
Tip: run make with `-j$(nproc)` for parallel compiling.

- `make` - builds tophat (located at `./tophat`).
- `make run` - builds tophat and runs it.
- `make clean` - clean repository after build.
- `make deepclean` - like `clean` but also clean submodules.
- `make cross` - build windows executable (located at `./tophat.exe`).
- `make bulk` - same as `make` but (re)compile all sources at once

## install instructions

### linux

1. Download the cli tool and tophat archive from releases.
2. Put the cli tool in path. (recommended)
3. `# tophat install tophat.zip`
4. `$ tophat check` to check whether tophat installed correctly

### windows

1. Download the windows archive from releases
2. Extract it wherever you like. Your projects will be stored there.
3. Read the readme.

## acknowledgements

- [vtereshkov](https://github.com/vtereshkov/) - umka
- [cnlohr](https://github.com/cnlohr) - rawdraw
- [nothings](https://github.com/nothings) - stb (image and ttf)
- [mackron](https://github.com/mackron) - miniaudio
