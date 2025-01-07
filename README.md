# gameboy-dmg-emulator
A simple C++ original gameboy (DMG) emulator for windows.

The issue for the copyright screen failing to show was ultimately an issue with the ADD_A_X opcode
implementation. Tetris uses jump tables to hop to functions based on the value stored in 0xFFE1. This is done
by calling rst 0x28 before defining the addresses of the functions in memory directly after the rst call. The
addresses are called such that 0 = first func, 1 = second func. However, the addresses are, of course, 2 bytes long,
which is why .dw is used to write them in the dissassembly. This means our index must be doubled to get the correct offset
in memory to call the function.

With that in mind, the call to display the credits is the 36th function in the function table. So in order to load
that, 0xFFE1 must be filled with 0x24, once doubled this would become 0x48, or 72 in decimal. Which would correctly point
to the function.

THE PROBLEM was that the value was doubled by being loaded into the A register, then calling ADD_A_A. Simple enough,
except that in my implementation ADD_A_X, didnt actually store the result back in the a register, resulting in nothing
happening.

Took like 2 weeks to fix.
