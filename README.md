# space-war
Space War is a shoot'em up game where you pilot a spaceship and your goal is to destroy all the enemies ship.

Trailer: [https://www.youtube.com/watch?v=EE7p4LESCMg](https://www.youtube.com/watch?v=EE7p4LESCMg)

Download Windows installer and updater : [https://gamejolt.com/games/get-the-crates/618857](https://gamejolt.com/games/get-the-crates/618857)

## Prerequisites to play
### Hardware
- **Screen resolution**: at least **1280 x 720 px** (= HD 720p), **1920 x 1080 px** recommended (= Full HD 1080p)
- **RAM**: about **200 Mo** (in addition to what the OS consumes) is necessary to run the game normally.
- **Storage**: about **65 Mo** of available space

### Software
- **Operating System** : Linux 64 bits, Windows 64 bits, macOS 64 bits
- If you are on Linux/macOS, the following **libraries** must be installed:
```
sdl2
sdl2_image
sdl2_gfx
sdl2_ttf
sdl2_mixer
sdl2_net
libcurl
```
## Play
Go to the web page: https://gamejolt.com/games/space-war/482884
## Build from sources:
### Clone
```shell
git clone https://github.com/gaziduc/space-war.git
cd space-war
cmake . # or use cmake-gui on Windows
make # or launch an compile the newly created Visual Studio solution / Codeblocks project on Windows
./space-war # or double-click space-war.exe on Windows
```

## Compile for emscripten on Windows

First of all, download make from chocolatey. Then do:
```shell
emcmake cmake . -G "Unix Makefiles"
emmake make
```
