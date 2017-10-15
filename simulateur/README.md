# Documentation

This directory contains the LEIA command line simulator. This
simulator can display (or not) a graphical screen that looks like a
"read-only terminal". 


## HOW TO COMPILE

- "make" will compile the simulator. 

Depends: 

The graphical screen needs the SDL Library and there seems to be an
issue with MAC OS X system. However, this graphical screen is not
really useful for our compiler, thus there is a conditional rule in
the makefile to compile without the SDL dependence:

WITHSDL=1  # comment to avoid SDL dependence.

(if you want to try the graphical screen, sudo apt-get install
libsdl2-dev)


## How to use:
To launch a simulation do:
`LEIA [-f  or -s or -r or -fulldebug or -q] file_name`

### options:
- `-f` will launch the simulator in 'fast mode'.
- `-s` will do simulate step by step, one instruction after the other. All the registers are printed at each step.
- `-fulldebug` goes inside a pseudo-gdb mode
- `-q`: quiet mode, no graphical screen, only the result of the simulation.

### Example:
./LEIA -f ../EX/test_print.obj

### Pseudo-gdb mode howto:
The commands can be inputted in the form \[X\]command where X is a facultative number. The command will be repeated N times.
The commands available are:
- s: execute one instruction
- n: execute one instruction but does not enter inside calls
- x address: will print the memory present between the addresses \[x-16, x+16\]
- Xp: X is an integer here. It will print the register X
- d: print all the registers and the PC


## config.rc:
This file can be use to input the cycles used by each instructions. The syntax of the file is as follow
```
instruction_name cycles
...
instruction_name cycles
```
instruction_name can be anything of add, sub, or, xor, and, lsl, lsr, asr, rmem, wmem, jump, call, return, copy, snif, letl, leth
If instruction_name is 'ticks', it will define the abstract constant which defines the wait time for a simulated tick. Beware, it has nothing to do with a second!
