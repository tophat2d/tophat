## Getting tophat

To download and install tophat, follow these instructions.

#### windows

Download the windows zip from releases. Extract it anywhere you like. This will be the main place containing all your projects. Something like gopath.

#### linux

Download the linux cli tool and add it to the path (recommended). Then run:
1. `tophat get`
2. `tophat install tophat.zip`

## Making new project

Although you can make projects by hand, it is much simpler to use the tool. This will initialize it with hello world preset.

#### linux

Navigate to the directory, where you want to have your project and use `tophat init`.

#### windows

Run the `init.bat` script and enter the name of your project.

## Running project

When you have the game set up, you can run the project.

#### linux

Use `tophat run` in the directory, your project is in.

#### windows

In the folder, where your game is located, there is a file called `run.bat`. Just open it.

## staying up-to-date

Tophat gets new features all the time. You might want to update it sometimes.

#### linux

Linux cli tool has automatic updater, that can be ran using `tophat update`. It automatically updates to the newest versions of tophat (unstable). I want to add option to only update stable versions.


#### windows

Windows currently doesn't have a way to update itself. However, you can compare the version file to the one located in `bin/version`. Then you can download the win package and replace it.
...

Congrats! You have achieved hello world. Now you can browse the [examples](https://github.com/marekmaskarinec/tophat/tree/main/examples), or read more [docs](https://github.com/marekmaskarinec/tophat/wiki/Umka-libraries).

