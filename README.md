# space-war
Space War is a shoot'em up game where you pilot a spaceship and your goal is to destroy all the enemies ship.

Play/Download the game here : [https://gaziduc.itch.io/space-war](https://gaziduc.itch.io/space-war)

## Prerequisites to play
### Hardware
- **Screen resolution**: at least **1280 x 720 px** (= HD 720p), **1920 x 1080 px** recommended (= Full HD 1080p)
- **RAM**: about **200 Mo** (in addition to what the OS consumes) is necessary to run the game normally.
- **Storage**: about **65 Mo** of available space

### Software
- **Operating System** : Windows 64 bits, Linux 64 bits, macOS 64 bits
- If you are on Linux/macOS, the following **libraries** must be installed: sdl2, sdl2_image, sdl2_gfx, sdl2_ttf, sdl2_mixer, sdl2_net, libcurl

## Build from sources:
```shell
git clone https://github.com/gaziduc/space-war.git
cd space-war
cmake . # or use cmake-gui on Windows
make # or launch an compile the newly created Visual Studio solution / Codeblocks project on Windows
./space-war # or double-click space-war.exe on Windows
```

## Build for emscripten

First of all, if you're on Windows, download make from chocolatey.
Then :
```shell
emcmake cmake . -G "Unix Makefiles"
emmake make
```
