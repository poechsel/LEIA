# LEIA
LEIA is a virtual asm created for some CS course. This project contain an assembly program, a simulator, and some basics assembly functions. A debugging GUI was recently added.

This directory contains LEIA, the simulator for the ASR1 course @ENSL,
year 2016.
Authors : Pierre Oechsel and Guillaume Duboc, ENSL.

## CONTENT
- the leia assembler, in assembleur/
- a command line simulator & debuguer, in simulateur/
- (optional) a graphical user interface, in gui/
- examples, in EX/

## INSTALL - PREREQUISITES
- in the simulator directory, read the README.md (you may or not
install the SDL2 graphical library)
- python3 for the assembly.
- (optional) Qt-5.9 for the GUI.

## ASSEMBLE & EXEC
- python3 asm.py /path/to/example.s produces /path/to/example.obj
    - the assembly program contains several options:
        - -h to view the help
        - -o OUT to specify the output
        - -d to generate a file containing debugging informations
        - --reverse_stack to reverse the stack direction
- then `./LEIA {-f,-s,-q} /path/to/example.obj`


## TEST DISTRIB
cd EX/

#### simple simulation step by step:
`python3 ../asm.py test_print.s && ../LEIA -s test_print.obj`

#### fast simu, graphical example
`python3 ../asm.py hello.s && ../LEIA -f hello.obj`

## ISA:
The ISA of our asm is in the course documentation.
