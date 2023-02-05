# Tundra
## Development instructions
### Compiling
* Either download CEmu Ti emulator at [Cemu Download](https://ce-programming.github.io/CEmu/download) or download the official [TI Connect CE]()
* Acquire a Ti-84 CE T rom from your own calculator or download from somewhere. Then select it when setting up CEmu
* Install the CE c toolchain from [CE toolchain](https://ce-programming.github.io/toolchain/static/getting-started.html#getting-started) and follow their guide to set up.
* Clone or download this repository and place it somewhere. 
* Open a terminal and navigate to the root of the folder. 
* Run `make gfx` and then `make` to create the binaries.

### Running
* Select the .8xp file from the newly created /bin folder and send it to a real calculator with TI connect CE or with 'send file to calc' in CEmu.

## Features
* Graphics from free use itch.io assets
* A world created from a tileset
* Animated movement of the player and tilemap
* A menu with submenus and the option to quit or continue
* An enemy that moves randomly, avoiding walls and obstacles, that kills you if you touch it
* A game over menu that forces you to quit the game
* An animated attack with a sword that kills the enemy
* If the enemy is killed, respawn it somewhere on the tilemap where the player can reach it
* If the player cannot find or kill the enemy, respawn it in a new location
* An indicator to show in what direction the enemy currently is, calculated with some math
* A score counter based on how many enemies you have killed

### Demonstration on CEmu
