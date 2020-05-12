#include "SDL_Func.hpp"
#include "Chip8_Arch.hpp"

// Initialize Chip-8 architecture.
chip8_arch Chip8;

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

	// Main game loop...
	while(!quit) {
		printf("Opcode: 0x%x\n", Chip8.temp_op);
		Chip8.emulateCycle();

		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}else if (e.type == SDL_KEYDOWN) {
				switch(e.key.keysym.sym) {
					case SDLK_ESCAPE:
						quit = true;
						break;

					/*case SDLK_0:
						SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
						break;*/
				}
			}
		}
	}

	// Deallocate resources and close SDL.
	close();

	return(0);

}
