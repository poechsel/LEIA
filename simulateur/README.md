# Documentation

## How to use:
To launch a simulation do:
`LEIA [-f or -skip or -s or -r or -fulldebug] file_name`

### options:
- `-f` will launch the simulator in 'fast mode': nothind is displayed, we are just simulating the processor
- `-s` will do simulate step by step, one instruction after the other. All the registers are printed at each step.
- `-skip` is like `-s` but it will not print informations inside calls.
- `-r` is like `f` but prints informations after every instructions. Not very usefull.
- `-fulldebug` goes inside a pseudo-gdb mode

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
