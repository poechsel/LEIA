#documentation about the library

## Fonts
The file fonts.s contains some font datas compatible with the putchar method in vfx.x

## Graphical library

### Rendering in LEIA
LEIA has a 160x128 screen which is projected to the top of the memory (from address 0xb000 to 0xffff). Every pixel is 16bits.
The pixel at address 0xb000 is the pixel at coordinates (0, 0) (bottom left of the screen). 0xfff is the pixel upper right.
Memory encoding is as follows rrrrrrgggggbbbbb. The last 5 bits are for the blue component, next 5 for the green component, the rest for the red component.

We use the instruction `refresh` to tell the simulator that the screen can be refreshed. 

### Functions inside the library
I give here a listing of functions implemented inside vfx.s. 
- putstr: put a string on a screen. A string is null-terminated, like in C
    - **erase:** r1, r2, r3, r4, r5, r6, r9, r10, r11, r12, r13, r14, r15
    - r0: color
    - r1: x coordinate
    - r2: y coordinate
    - r3: pointer to the string
    - r4: if r4 is 1, do not refresh. Otherwise refresh between each letter (for a typing effect)
- putchar: put a char on the screen
    - **erase:** r1, r2, r3, r4, r5, r6
    - r0: color
    - r1: x coordinate
    - r2: y coordinate
    - r3: char value
- line: draw a line going from (x1, y1) to (x2, y2)
    - **erase:** r1, r2, r3, r4, r5, r6, r8, r9
    - r0: color
    - r1: x1
    - r2: y1
    - r3: x2
    - r4: y2
- plotpx: Plot the pixel at the coordinates (x, y) with the color color
    - **erase:** r2, r3
    - r0: color
    - r1: x
    - r2: y
- clearscr: Fill the screen with a color
    - **erase:** r1
    - r0: color
- fill: Fill a rectangle goign from (x1, y1) to (x2, y2)
    - **erase:** r1, r2, r3, r4, r5
    - r0: color
    - r1: x1
    - r2: y1
    - r3: x2
    - r4: y2

