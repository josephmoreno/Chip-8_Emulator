// To build single file HTML:
// em++ -std=gnu++17 -Wall chip8.cpp sdl_func.cpp chip8_arch.cpp -o chip8.html --use-port=sdl2 --use-port=sdl2_image:formats=png -sUSE_SDL_MIXER=2 -sSDL2_MIXER_FORMATS=wav,mp3 -sSINGLE_FILE --embed-file debug/ROMs@ROMs --embed-file debug/Music@Music

// Build with only BRIX ROM:
// em++ -std=gnu++17 -Wall chip8.cpp sdl_func.cpp chip8_arch.cpp -o chip8.html --use-port=sdl2 --use-port=sdl2_image:formats=png -sUSE_SDL_MIXER=2 -sSDL2_MIXER_FORMATS=wav,mp3 -sSINGLE_FILE --embed-file debug/ROMs/BRIX@ROMs/BRIX --embed-file debug/Music@Music

// Build for ES6 (was able to use in Vite React app):
// em++ -std=gnu++17 -Wall chip8.cpp sdl_func.cpp chip8_arch.cpp -o chip8.mjs --use-port=sdl2 --use-port=sdl2_image:formats=png -sUSE_SDL_MIXER=2 -sSDL2_MIXER_FORMATS=wav,mp3 -sMODULARIZE=1 -sEXIT_RUNTIME=1 --embed-file debug/ROMs/BRIX@ROMs/BRIX --embed-file debug/Music@Music
// * -sMODULARIZE=1 and .mjs output file extension; .mjs implicitly enables EXPORT_ES6 flag
// * Default module name is 'Module'; use EXPORT_NAME=<different name> flag to use change from the default
// * -sEXIT_RUNTIME=1 for enabling forced termination of the program
// * https://emscripten.org/docs/tools_reference/settings_reference.html

// Build for ES6, export normalQuit to terminate program:
// em++ -std=gnu++17 -Wall chip8.cpp sdl_func.cpp chip8_arch.cpp -o chip8.mjs --use-port=sdl2 --use-port=sdl2_image:formats=png -sUSE_SDL_MIXER=2 -sSDL2_MIXER_FORMATS=wav,mp3 -sMODULARIZE=1 -sEXPORTED_FUNCTIONS=_main,_normalQuit -sEXPORTED_RUNTIME_METHODS=cwrap -sEXIT_RUNTIME=1 --embed-file debug/ROMs/BRIX@ROMs/BRIX --embed-file debug/Music@Music

#include "sdl_func.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// Initialize Chip-8 architecture.
Chip8Arch Chip8;

// Initalize SDL object
SdlObj SdlObj0;

// Main loop flag to see if the player
// has quit the game.
bool quit = false;

// The SDL event handler.
SDL_Event e;

// Create timers and frame counter.
Timer fps_timer;
Timer frame_cap_timer;
int frame_counter = 0;
float avg_FPS = 0;

static void mainLoop(void);

int main(int argc, char *argv[]) {
	// Variables used for loading a ROM.
	FILE *file = NULL;
	// std::string ROM;

	// // Asking the user for a ROM name.
	// std::cout << "Enter the name of the game you want to play or enter \"Q\" to exit: ";
	// std::cin >> ROM;

	// if (ROM == "Q" || ROM == "q") return(0);

	// ROM.insert(0, "ROMs\\");
	// file = fopen(ROM.c_str(), "rb");

	// while (file == NULL) {
	// 	std::cout << "Could not find ROM by that name.\n";
	// 	std::cout << "Enter the name of the game you want to play or enter \"Q\" to exit: ";
	// 	std::cin >> ROM;

	// 	if (ROM == "Q" || ROM == "q") return(0);

	// 	ROM.insert(0, "ROMs\\");
	// 	file = fopen(ROM.c_str(), "rb");
	// }

	file = fopen("ROMs/BRIX", "rb");

	// Determine the ROM's size.
	fseek(file, 0L, SEEK_END);
	unsigned int file_size = ftell(file);
	fseek(file, 0L, SEEK_SET);

	// Initialize the Chip-8.
	Chip8.initialize();

	// Create buffer of the ROM's contents to load into the memory.
	std::vector<unsigned char> buffer(file_size, 0x00);
	unsigned int x;

	for(x = 0; x < file_size; ++x) {
		buffer[x] = fgetc(file);
		//printf("0x%x\n", buffer[x]);
	}

	fclose(file);

	// Load the ROM into memory.
	Chip8.loadROM(buffer);

	// Starting up SDL and creating a window.
	if (!SdlObj0.init()) {
		return(1);
	}else {
		// Load the media.
		if (!SdlObj0.loadMedia()) {
			return(1);
		}
	}

	// Start fps_timer
	fps_timer.start();

	// Frame limiter.
	//unsigned int frame_limiter = 500000;

	// Main game loop...
	// while(!quit) {
	// 	// DEBUGGING
	// 	/*if (frame_limiter < 500000) {
	// 		++frame_limiter;
	// 		continue;
	// 	}else
	// 		frame_limiter = 0;*/
		
	// 	// Start the cap timer for each cycle.
	// 	frame_cap_timer.start();

	// 	// Calculate the average FPS.
	// 	avg_FPS = (frame_counter / (fps_timer.getTicks() / 1000.f));
	// 	if (avg_FPS > 2000000)
	// 		avg_FPS = 0;

	// 	// Reset frame counter and the FPS timer if frame_counter == 60.
	// 	if (frame_counter >= 60) {
	// 		frame_counter = 0;
	// 		fps_timer.stop();
	// 		fps_timer.start();
	// 	}

	// 	// Emulate a cycle of the Chip-8 architecture.
	// 	Chip8.emulateCycle();

	// 	// Play sound effect if sound register > 0.
	// 	if (Chip8.s_reg > 0x00) {
	// 		Mix_PlayChannel(-1, SdlObj0.chuu_warai, 0);
	// 	}
		
	// 	// DEBUGGING BY SEEING ARCHITECTURE CONTENTS.	
	// 	//Chip8.printContents();
	// 	//std::cin.get();

	// 	// Render and update the screen if Chip8.dispFlag == 0x01.
	// 	if (Chip8.dispFlag == 0x01) {
	// 		SdlObj0.drawScreen(Chip8);
	// 		++frame_counter;
	// 	}

	// 	// Delay until the cap timer >= MS_PER_FRAME.
	// 	while (frame_cap_timer.getTicks() < SdlObj0.MS_PER_FRAME) {}
	// 	frame_cap_timer.stop();

	// 	// // Take input from the player.
	// 	// HERE IS WHERE THAT HUGE WHILE+IF+SWITCH STATEMENT GOES
	// }

	// // Deallocate resources and close SDL.
	// SdlObj0.close();

	#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainLoop, 0, 1);
    #else
    while(1) { mainLoop(); }
    #endif

	return(0);
}

static void mainLoop(void) {
    if (quit) {
        SdlObj0.close();
        #ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop();
        #else
        exit(0);
        #endif
    }

	// DEBUGGING
	/*if (frame_limiter < 500000) {
		++frame_limiter;
		continue;
	}else
		frame_limiter = 0;*/
	
	// Start the cap timer for each cycle.
	frame_cap_timer.start();

	// Calculate the average FPS.
	avg_FPS = (frame_counter / (fps_timer.getTicks() / 1000.f));
	if (avg_FPS > 2000000)
		avg_FPS = 0;

	// Reset frame counter and the FPS timer if frame_counter == 60.
	if (frame_counter >= 60) {
		frame_counter = 0;
		fps_timer.stop();
		fps_timer.start();
	}

	// Emulate a cycle of the Chip-8 architecture.
	Chip8.emulateCycle();

	// Play sound effect if sound register > 0.
	if (Chip8.s_reg > 0x00) {
		Mix_PlayChannel(-1, SdlObj0.chuu_warai, 0);
	}
	
	// DEBUGGING BY SEEING ARCHITECTURE CONTENTS.	
	//Chip8.printContents();
	//std::cin.get();

	// Render and update the screen if Chip8.dispFlag == 0x01.
	if (Chip8.dispFlag == 0x01) {
		SdlObj0.drawScreen(Chip8);
		++frame_counter;
	}

	// Delay until the cap timer >= MS_PER_FRAME.
	while (frame_cap_timer.getTicks() < SdlObj0.MS_PER_FRAME) {}
	frame_cap_timer.stop();

    // Take input from the player.
	while (SDL_PollEvent(&e) != 0) {

		// Top-right exit button...
		if (e.type == SDL_QUIT) {
			quit = true;

		// Key pressed...
		}else if (e.type == SDL_KEYDOWN) {
			Chip8.keyDown = 0x01;

			switch(e.key.keysym.sym) {
				case SDLK_ESCAPE:
					quit = true;
					break;

				case SDLK_1:
					Chip8.keys[0x01] = 0x01;
					Chip8.keyPressed = 0x01;
					break;

				case SDLK_2:
					Chip8.keys[0x02] = 0x01;
					Chip8.keyPressed = 0x02;
					break;

				case SDLK_3:
					Chip8.keys[0x03] = 0x01;
					Chip8.keyPressed = 0x03;
					break;

				case SDLK_4:
					Chip8.keys[0x0c] = 0x01;
					Chip8.keyPressed = 0x0c;
					break;

				case SDLK_q:
					Chip8.keys[0x04] = 0x01;
					Chip8.keyPressed = 0x04;
					break;

				case SDLK_w:
					Chip8.keys[0x05] = 0x01;
					Chip8.keyPressed = 0x05;
					break;

				case SDLK_e:
					Chip8.keys[0x06] = 0x01;
					Chip8.keyPressed = 0x06;
					break;

				case SDLK_r:
					Chip8.keys[0x0d] = 0x01;
					Chip8.keyPressed = 0x0d;
					break;

				case SDLK_a:
					Chip8.keys[0x07] = 0x01;
					Chip8.keyPressed = 0x07;
					break;

				case SDLK_s:
					Chip8.keys[0x08] = 0x01;
					Chip8.keyPressed = 0x08;
					break;

				case SDLK_d:
					Chip8.keys[0x09] = 0x01;
					Chip8.keyPressed = 0x09;
					break;

				case SDLK_f:
					Chip8.keys[0x0e] = 0x01;
					Chip8.keyPressed = 0x0e;
					break;

				case SDLK_z:
					Chip8.keys[0x0a] = 0x01;
					Chip8.keyPressed = 0x0a;
					break;

				case SDLK_x:
					Chip8.keys[0x00] = 0x01;
					Chip8.keyPressed = 0x00;
					break;

				case SDLK_c:
					Chip8.keys[0x0b] = 0x01;
					Chip8.keyPressed = 0x0b;
					break;

				case SDLK_v:
					Chip8.keys[0x0f] = 0x01;
					Chip8.keyPressed = 0x0f;
					break;

				// For playing/pausing music.
				case SDLK_p:

					// If no music is active...
					if (Mix_PlayingMusic() == 0) {

						// ...then play the music.
						Mix_PlayMusic(SdlObj0.music, -1);

					// If music is already active...
					} else{

						// ...if it's paused...
						if (Mix_PausedMusic() == 1) {

							// ...then resume it.
							Mix_ResumeMusic();

						// ...if it's playing...
						}else {

							// ...then pause it.
							Mix_PauseMusic();
						}
					}

					break;
			}

		// Key released...
		}else if (e.type == SDL_KEYUP) {
			switch(e.key.keysym.sym) {
				case SDLK_1:
					Chip8.keys[0x01] = 0x00;
					break;

				case SDLK_2:
					Chip8.keys[0x02] = 0x00;
					break;

				case SDLK_3:
					Chip8.keys[0x03] = 0x00;
					break;

				case SDLK_4:
					Chip8.keys[0x0c] = 0x00;
					break;

				case SDLK_q:
					Chip8.keys[0x04] = 0x00;
					break;

				case SDLK_w:
					Chip8.keys[0x05] = 0x00;
					break;

				case SDLK_e:
					Chip8.keys[0x06] = 0x00;
					break;

				case SDLK_r:
					Chip8.keys[0x0d] = 0x00;
					break;

				case SDLK_a:
					Chip8.keys[0x07] = 0x00;
					break;

				case SDLK_s:
					Chip8.keys[0x08] = 0x00;
					break;

				case SDLK_d:
					Chip8.keys[0x09] = 0x00;
					break;

				case SDLK_f:
					Chip8.keys[0x0e] = 0x00;
					break;

				case SDLK_z:
					Chip8.keys[0x0a] = 0x00;
					break;

				case SDLK_x:
					Chip8.keys[0x00] = 0x00;
					break;

				case SDLK_c:
					Chip8.keys[0x0b] = 0x00;
					break;

				case SDLK_v:
					Chip8.keys[0x0f] = 0x00;
					break;
			}
		}
	}
};

extern "C" {
	void normalQuit() {
		SdlObj0.close();
        #ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop();
        #else
        exit(0);
        #endif
	}

	// Simple example for troubleshooting
	// int add(int x, int y) {
	// 	return(x + y);
	// }
};