# space-war
Space War is a shoot'em up game where you pilot a spaceship and your goal is to destroy all the enemies ship.

![](https://s5.gifyu.com/images/2019-12-29-002144_1920x1080_scrot.png)
## Prerequisites to play
### Hardware
- **Screen resolution**: at least **1280 x 720 px** (= HD 720p), **1920 x 1080 px** recommended (= Full HD 1080p)
- **RAM**: about **100 Mo** (in addition to what to OS consumes) is necessary to run the game normally.
- **Storage**: about **30 Mo** of available space
### Software
- **Operating System** : Linux 64 bits, Windows 64 bits (maybe Mac in the future)
- If you are on Linux, the following **libraries** must be installed:
```
sdl2
sdl2_image
sdl2_gfx
sdl2_ttf
sdl2_mixer
sdl2_net
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
