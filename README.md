# Chip-8 Emulator

*** These sources heavily referenced:\
Chip-8 Information:
- http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
- http://emulator101.com/
- http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#1.0
- https://en.wikipedia.org/wiki/CHIP-8#Opcode_table

SDL Information:
- http://lazyfoo.net/tutorials/SDL/index.php#Texture%20Streaming

The extension libraries SDL_image and SDL_mixer are needed for compilation.\
I followed SDL tutorial "Lesson 06" (from the above link ^) to download and place the files in the correct directories for my compiler.\
\
*** Purpose of this project:\
Gain experience working with a library (SDL) that handles video/audio output and keystroke input.\
\
Gain a better general understanding of computer architecture and how to emulate the hardware as software.\
\
Another reason for undertaking this project is my interest in video games and
emulators.\
\
*** Compilation:
All code compiled using MinGW.\
Compilation command used is in the .bat file.\
\
SDL_image and SDL_mixer are needed for compilation.\
Please see the "SDL Information" reference (at the top ^).\
\
I compile and generate a .exe file for every push.\
\
*** How to use:\
1.) Run .exe file.\
2.) Type in a name of one of the games included in the "ROMs" folder.\
3.) Chip-8 controls have been mapped to the following keyboard input:
<pre><code>

    Chip-8 Keypad	==	Keyboard Input
    -----------------		-----------------
    | 1 | 2 | 3 | C |		| 1 | 2 | 3 | 4 |
    -----------------		-----------------
    | 4 | 5 | 6 | D |		| Q | W | E | R |
    -----------------		-----------------
    | 7 | 8 | 9 | E |		| A | S | D | F |
    -----------------		-----------------
    | A | 0 | B | F |		| Z | X | C | V |
    -----------------		-----------------

</code></pre>
    To play music, press "P".
