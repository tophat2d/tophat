# tophat

![](etc/icon.png)

Tophat is a friendly 2d game framework for the
[Umka](https://github.com/vtereshkov/umka-lang) programming language. It offers
primitives for 2d graphics, audio and user input as well as high-level modules
like user interfaces, particle systems or tilemaps. Tophat can run on Windows
and Linux machines as well as in a web browser.

* [Website](https://tophat2d.dev)
* [Downloads](https://tophat2d.dev/dl)
* [API reference](https://docs.tophat2d.dev)
* [Samples](https://github.com/tophat2d/tophat/tree/main/samples)
* [Demos](https://github.com/tophat2d/tophat/tree/main/demos)

## screenshots

<p align="center">
	<img
		src="https://tophat2d.dev/img/space-shooter.png"
		alt="space shooter example"
	>
	<img
		src="https://img.itch.zone/aW1hZ2UvMTA4MzEwMS82MjA4NzA5LnBuZw==/347x500/qNlZNI.png"
		alt="logs together jam game"
	>
	<img
		src="https://tophat2d.dev/img/tetris.png"
		alt="tetris game"
	>
	<img
		src="https://tophat2d.dev/img/pomodoro.png"
		alt="pomodoro app"
	>
</p>

## build instructions

1. Clone the repository with the `--recursive` flag.

```
git clone https://github.com/tophat2d/tophat --recursive
```

2. On Linux run `make`, on Windows `msbuild.bat`.

You can also download precompiled binaries [here](https://github.com/tophat2d/tophat/releases).

## contributing

I'm happy to take contributions through the mailing list. Please format your c
files using the `.clang-format` file. If you contribute umka code, please
format it similarly to other code.

## license

Tophat is licensed under the BSD-3 license.
