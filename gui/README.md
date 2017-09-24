# GUI

## Build :
Do not regenerate the `.pro`. It compiles using **Qt5**, not Qt4.
Two options to build it :
- Use QtCreator and open the `.pro`
- Generate a makefile with Qt: `qmake`. Then make: `make`

## Informations

This debugger show several informations :
- a dump of the memory (printed in dec, hex and char formats)
- a dump of the registers
- a disassembly of the memory to read the code in a more human format. If a `.debug` file is provided along with the `.obj` file (generated using `--debug` option of the assembly program), it will use the informations it provides to enhance the disassembly (mostly adding labels)

You can also edit the contents of the memory and the registers. With the right clic you can jump between the different views (registers -> memory and memory <-> code)

Breakpoints can also be added (thanks to the checkboxes in the code view). The code can be executed by different way :
- step by step (`single`)
- step by step where the calls are considered as a single step (`next`)
- Until the next breakpoint (`Break`)
- Until the end (`Play`)

