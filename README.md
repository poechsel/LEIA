#README

### Assembleur:
	- .let pour affecter une valeur à un registre
	- .reserve x créer x '.word 0' dans le fichier: sert à réserver de la place en mémoire
	- .push n sert à push le registre n. Est équivalent à wmem n [r7] ; add r7 r7 1
	- .pop n pop sur le registre n. Est équivalent à sub r7 r7 1 ; rmem n [r7]


### ASM
	- le fichier vfx.s contient les fonctions graphiques requises
	- main.obj est le fichier assemblé de notre démo graphique
	- le code de la démo graphique est dans le sous-dossier demo


### Simulateur
- a besoin de la SDL2 (sous linux: sudo apt-get install libsdl2-dev pour l'installer)
- faire make pour compiler, puis sudo make install pour mettre le fichier binaire en accès direct
- en plus des options s/f/r, j'en ai rajouté une autre: fulldebug pour lancer un débugger

### Demo:
- Pour lancer la démo il est préférable d'utiliser notre simulateur. En effet, la commande refresh permet d'avoir une sortie écran synchronisée avec le programme. De plus nous l'avons uniquement testé sous notre simulateur. 


###Misc:
- asm_c.py correspond à un programme permettant de 'convertir' un code asm en programme c++ pouvant être compilé avec g++ -O3 -std=c++11 -lpthread -lSDL2
	Ne fonctionne pas avec les nouvelles extensions que l'on a ajoutés (elles ne sont pas implementées). Ne permet pas de réecrire le programme pendant son éxecution. Est éxtremement rapide!
