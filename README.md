# space-war
Space War is a shoot'em up game where you pilot a spaceship and your goal is to destroy all the enemies ship.

Trailer: [https://www.youtube.com/watch?v=EE7p4LESCMg](https://www.youtube.com/watch?v=EE7p4LESCMg)

Download Windows installer and updater : [https://gaziduc.github.io/space-war/](https://gaziduc.github.io/space-war/)

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
```
## Play only
Go to the web page: https://gamejolt.com/games/space-war/482884
## Download, build and play:
### Clone
```shell
git clone https://github.com/gaziduc/space-war.git
cd space-war
```
### Generate project
```shell
cmake . # or use cmake-gui on Windows
```
### Build
```shell
make # or launch the newly created Visual Studio solution / Codeblocks project on Windows
```
### Launch
```shell
./space-war # or double-click space-war.exe on Windows
```
