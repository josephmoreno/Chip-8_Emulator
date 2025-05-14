echo Compiling...
g++ --std=gnu++17 -Wall -m64 -I "C:/Program Files/mingw64/sdl2-w64/include" *.cpp -L "C:/Program Files/mingw64/sdl2-w64/lib" -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -o debug/chip8
:: pause
.\debug\chip8
