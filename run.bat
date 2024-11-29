echo Compiling...
g++ -std=gnu++17 -Wall *.cpp -I "C:/Program Files/mingw64/sdl2-w64/include" -o debug/chip8 -L "C:/Program Files/mingw64/sdl2-w64/lib" -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer
pause
cd debug && .\chip8