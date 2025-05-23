#include "SDL_Func.hpp"
#include "Chip8_Arch.hpp"

// Initialize Chip-8 architecture.
chip8_arch Chip8;

// Function for drawing to the screen.
void drawScreen();

int main(int argc, char *argv[]) {

	// Variables used for loading a ROM.
	FILE *file = NULL;
	std::string ROM;

	// Asking the user for a ROM name.
	std::cout << "Enter the name of the game you want to play or enter \"Q\" to exit: ";
	std::cin >> ROM;

	if (ROM == "Q" || ROM == "q") return(0);

	ROM.insert(0, "ROMs\\");
	file = fopen(ROM.c_str(), "rb");

	while (file == NULL) {
		std::cout << "Could not find ROM by that name.\n";
		std::cout << "Enter the name of the game you want to play or enter \"Q\" to exit: ";
		std::cin >> ROM;

		if (ROM == "Q" || ROM == "q") return(0);

		ROM.insert(0, "ROMs\\");
		file = fopen(ROM.c_str(), "rb");
	}

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
	if (!init()) {
		return(1);
	}else {
		// Load the media.
		if (!loadMedia()) {
			return(1);
		}
	}

	// Main loop flag to see if the player
	// has quit the game.
	bool quit = false;

	// The SDL event handler.
	SDL_Event e;

	// FPS code
	const int fps = 500;
	const int frame_delay = 1000 / fps;
	Uint32 frame_start;
	int frame_time;

	// Frame limiter.
	//unsigned int frame_limiter = 500000;

	// Main game loop...
	while(!quit) {
		frame_start = SDL_GetTicks();

		// Emulate a cycle of the Chip-8 architecture.
		Chip8.emulateCycle();

		// Play sound effect if sound register > 0.
		if (Chip8.s_reg > 0x00) {
			Mix_PlayChannel(-1, chuu_warai, 0);
		}
		
		// DEBUGGING BY SEEING ARCHITECTURE CONTENTS.	
		//Chip8.printContents();
		//std::cin.get();

		// Render and update the screen if Chip8.dispFlag == 0x01.
		if (Chip8.dispFlag == 0x01)
			drawScreen();

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
							Mix_PlayMusic(music, -1);

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

		frame_time = SDL_GetTicks() - frame_start;

        if (frame_delay > frame_time)
            SDL_Delay(frame_delay - frame_time);
	}

	// Deallocate resources and close SDL.
	close();

	return(0);

}

void drawScreen() {
	unsigned int x;

	SDL_Rect pixel;

	// Clear the screen with black.
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(renderer);

	for(x = 0; x < Chip8.display.size(); ++x) {
		if (Chip8.display[x] == 0x01) {
			// Render the 10x10 "pixel" to the screen.
			pixel = {(((int)x % 64) * 10), (((int)x / 64) * 10), 10, 10};
			SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
			SDL_RenderFillRect(renderer, &pixel);
		}
	}

	// Update the screen.
	SDL_RenderPresent(renderer);

	// Set the display flag to 0x00.
	Chip8.dispFlag = 0x00;

	return;
}
