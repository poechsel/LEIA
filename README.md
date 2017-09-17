# LEIA
LEIA is a virtual asm created for some CS course. This project contain an assembly program, a simulator, and some basics assembly functions. A debugging GUI was recently added.



This directory contains LEIA, the simulator for the ASR1 course @ENSL,
year 2016.
Authors : Pierre Oechsel and Guillaume Duboc, ENSL.

## CONTENT
- a graphical simulator
- an assembly code
- examples

## INSTALL - PREREQUISITES
- the simulator needs the SDL2 graphical library (sudo apt-get install libsdl2-dev)
- "make" will compile the simulator. 
- python3 for the assembly

## ASSEMBLY
- python3 asm.py /path/to/example.s produces /path/to/example.obj
    - the assembly program contains several options:
        - -h to view the help
        - -o OUT to specify the output
        - -d to generate a file containing debugging informations
        - --reverse_stack to reverse the stack direction
- then ./LEIA {-f,-s,-q} /path/to/example.obj

## GUI
- The graphical debugger can be compiled with Qt-5.9

## TEST DISTRIB
cd EX 

#### simple simulation step by step:
python3 ../asm.py tp1-simple.s && ../LEIA -s tp1-simple.obj

#### fast simu, graphical example
python3 ../asm.py hello.s && ../LEIA -f hello.obj 


## ISA:
The ISA of our asm is disponible here:
http://perso.citi-lab.fr/fdedinec/enseignement/2016/isa2016.pdf
