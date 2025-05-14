#include <stdio.h>

#include "SDL2\SDL_image.h"

#include "sdl_func.hpp"

// START SdlObj struct implementation
bool SdlObj::init() {
	// This will be the return variable.
	bool success = true;

	// Initialize SDL to access SDL functions.
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		printf("Failed to initialize SDL. SDL Error: %s\n", SDL_GetError());
		success = false;
	}else {
		// Create a window.
		window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_UNDEFINED,
					  SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
					  SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

		if (window == NULL) {
			printf("Failed to create window. SDL Error: %s\n", SDL_GetError());
			success = false;
		}else {
			// Create renderer for the window.
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

			if (renderer == NULL) {
				printf("Renderer failed to be created. SDL Error: %s\n", SDL_GetError());
				success = false;
			}else {
				// Initialize renderer color to black.
				SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);

				// Initialize .png loading.
				int imgFlags = IMG_INIT_PNG;

				if (!(IMG_Init(imgFlags) & imgFlags)) {
					printf("SDL_image failed to initialize. SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
				
				// Initialize SDL_mixer.
				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
					printf("SDL_mixer failed to initialize. SDL_mixer Error: %s\n", Mix_GetError());
					success = false;
				}else {
					// Get the window's surface.
					screenSurface = SDL_GetWindowSurface(window);
				}
			}
		}
	}

	return success;
}

bool SdlObj::loadMedia() {
	// ...return variable.
	bool success = true;

	// Load the music in.
	music = Mix_LoadMUS("Music/Casiopea - Mint Jams (Album).mp3");
	if (music == NULL) {
		printf("Failed to load music. SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}

	// Load the SFX in.
	chuu_warai = Mix_LoadWAV("Music/chuu_warai.wav");
	if (chuu_warai == NULL) {
		printf("Failed to load sound effect. SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}

	return success;
}

void SdlObj::drawScreen(Chip8Arch Chip8) {
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

void SdlObj::close() {
	// Deallocate the surfaces loaded.
	/*for(int x = 0; x < KEY_PRESS_SURFACE_TOTAL; ++x) {
		SDL_FreeSurface(keyPressSurfaces[x]);
		keyPressSurfaces[x] = NULL;
	}*/

	// Deallocate sound effects.
	Mix_FreeChunk(high_SFX);
	Mix_FreeChunk(med_SFX);
	Mix_FreeChunk(low_SFX);
	Mix_FreeChunk(chuu_warai);
	high_SFX = NULL;
	med_SFX = NULL;
	low_SFX = NULL;
	chuu_warai = NULL;

	// Deallocate music.
	Mix_FreeMusic(music);
	music = NULL;

	// Deallocate texture.
	if (texture != NULL) SDL_DestroyTexture(texture);
	texture = NULL;

	// Deallocate the renderer and window.
	if (renderer != NULL) SDL_DestroyRenderer(renderer);
	if (window != NULL) SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;

	// Close the SDL subsystems.
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();

	return;
}

SDL_Surface *SdlObj::loadSurface(std::string path) {
	// An optimized image to display (as opposed to a raw image).
	SDL_Surface *optimizedSurface = NULL;

	// Load the image to loadedSurface using SDL_image.
	SDL_Surface *loadedSurface = IMG_Load(path.c_str());

	if (loadedSurface == NULL) {
		printf("Failed to load image %s. SDL Error: %s\n", path.c_str(), SDL_GetError());
	}else {
		// Convert surface to screen format.
		optimizedSurface = SDL_ConvertSurface(loadedSurface, screenSurface->format, 0);

		if (optimizedSurface == NULL) {
			printf("Failed to optimize image %s. SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		// Deallocate old surface (loadedSurface).
		SDL_FreeSurface(loadedSurface);
	}

	return optimizedSurface;
}

SDL_Texture *SdlObj::loadTexture(std::string path) {
	// The final texture
	SDL_Texture *newTexture = NULL;

	// Load image from a specified path.
	SDL_Surface *loadedSurface = IMG_Load(path.c_str());

	if (loadedSurface == NULL) {
		printf("Failed to load image %s. SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}else {
		// Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);

		if (newTexture == NULL) {
			printf("Failed to create texture from %s. SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		// Deallocate loadedSurface.
		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
}
// END SdlObj struct implementation