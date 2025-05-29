:: pushd .
:: cd "C:\Program Files\emsdk"
:: Set-ExecutionPolicy RemoteSigned -Scope CurrentUser
:: .\emsdk_env
:: popd

cd %~dp0
em++ -std=gnu++17 -Wall chip8.cpp sdl_func.cpp chip8_arch.cpp -o chip8.mjs --use-port=sdl2 --use-port=sdl2_image:formats=png -sUSE_SDL_MIXER=2 -sSDL2_MIXER_FORMATS=wav,mp3 -sMODULARIZE=1 -sEXPORTED_FUNCTIONS=_main,_normalQuit -sEXPORTED_RUNTIME_METHODS=cwrap -sEXIT_RUNTIME=1 --embed-file assets@assets --embed-file debug/ROMs@ROMs --embed-file debug/Music@Music