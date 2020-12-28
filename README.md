# space-war
Space War is a shoot'em up game where you pilot a spaceship and your goal is to destroy all the enemies ship.
[![Watch Trailer](https://imgur.com/qD2PXpr)](https://www.youtube.com/watch?v=gADgjABsoNw)

## Prerequisites to play
### Hardware
- **Screen resolution**: at least **1280 x 720 px** (= HD 720p), **1920 x 1080 px** recommended (= Full HD 1080p)
- **RAM**: about **200 Mo** (in addition to what to OS consumes) is necessary to run the game normally.
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
## Play only
Go to the web page: https://gamejolt.com/games/space-war/482884
## Download, build and play:
```shell
git clone https://github.com/gaziduc/space-war.git
cd space-war
cmake .
make
./space-war
```
