# tophat

Tophat is a 2d game engine focused on minimalism using [Umka](https://github.com/vtereshkov/umka-lang), a statically typed scripting language for scripting. Games can however be extended with c.

Tophat is currently going through a big rework. I don't advise to use it for a project as there will be a lot of breaking changes. See the progress [here](./rewrite.md).

## features

- easy to use
- very lightweight
- minimalist ui framework
- automatic scaling - your games look the same no matter the resolution
- great cli tool for linux

## getting started

If you just want to see, how a game made using tophat looks like, you can find some in the `examples` folder.
Standard library documentation can be found [here](https://marekmaskarinec.github.io/thdocs/index.html).

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

## special thanks

TODO
