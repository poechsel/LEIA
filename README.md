# LEIA
LEIA is a virtual asm created for some CS course. This project contain an assembly program, a simulator, and some basics assembly functions



This directory contains LEIA, the simulator for the ASR1 course @ENSL,
year 2016.
Authors : Pierre Oechsel and Guillaume Duboc, ENSL.

#CONTENT
- a graphical simulator
- an assembly code
- examples

#INSTALL - PREREQUISITES
- the simulator needs the SDL2 graphical library (sudo apt-get install libsdl2-dev)
- "make" will compile the simulator. 
- python3 for the assembly

#ASSEMBLY
- python3 asm.py /path/to/example.s produces /path/to/example.obj
- then ./simu [f or s] /path/to/example.obj

#TEST DISTRIB
cd EX 
## simple simulation step by step:
python3 ../asm.py tp1-simple.s && ../simu -s tp1-simple.obj
## fast simu, graphical example
python3 ../asm.py hello.s && ../simu -f hello.obj 
