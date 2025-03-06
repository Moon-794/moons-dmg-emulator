# Moons Gameboy Emulator
A simple C++ original gameboy (DMG) emulator for windows.
My first attempt at an emulator, it is far from cycle accurate but is still able to
practically emulate a variety of games.

This project is still very much a work in progress, it will probably require a large refactor
as I simply didnt know enough about emulator design to keep things clean as the codebase grew
in size.

# Features
* Functional CPU with all 512 CPU opcodes implemented
* Implemented V-Blank, Serial and STAT interrupts
* Picture Processing Unit

# GIFS
<img src="./preview/Logo.gif" width="35%" height="35%"/>
<img src="./preview/Tetris.gif" width="35%" height="35%"/>
<img src="./preview/Tennis.gif" width="35%" height="35%"/>
<img src="./preview/SML.gif" width="35%" height="35%"/>


|||
|-------------|-------------|
|<img src="./preview/Logo.gif" width="35%" height="35%"/>|<img src="./preview/Tetris.gif" width="35%" height="35%"/>|
|<img src="./preview/Tennis.gif" width="35%" height="35%"/>|<img src="./preview/SML.gif" width="35%" height="35%"/>|

# Test ROMS
## BLAARG CPU Test Roms


| No. |   Name  |   Passed?  |
| --- | ------- | ---------- |
|  1  | Special            |✓|
|  2  | Interrupts         |X|
|  3  | OP SP,HL           |✓|
|  4  | OP R, IMM          |✓|
|  5  | OP RP              |✓|
|  6  | OP LD R,R          |✓|
|  7  | JR,JP,CALL,RET,RST |✓|
|  8  | MISC               |✓|
|  9  | OP R,R             |✓|
|  10 | BIT OPS            |✓|
|  11 | OP A,(HL)          |✓|


# How to run
1) `mkdir build`
2) `cd build`
3) `cmake -GNinja ..`
4) `ninja`
5) `cd bin`
7) Aquire a valid DMG bootrom, and game ROM, tetris is a good shout :)
8) In the console, write `./emulator.exe [BOOTROM_FILE] [GAME_ROM_FILE]`
