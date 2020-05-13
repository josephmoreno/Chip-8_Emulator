#include <iostream>
#include <cstdio>
#include <stdio.h>

#include "SDL2\SDL.h"
#include "SDL2\SDL_image.h"

// The main window and its surface.
SDL_Window *window = NULL;
SDL_Surface *screenSurface = NULL;

// Surface(s) used to load images to the main window's surface.
SDL_Surface *currentSurface = NULL;

// Texture variables.
SDL_Renderer *renderer = NULL;	// Window renderer.
SDL_Texture *texture = NULL;	// Currently displayed texture.

int SCREEN_WIDTH = 640, SCREEN_HEIGHT = 320;

bool init() {
	// This will be the return variable.
	bool success = true;

	// Initialize SDL to access SDL functions.
	if (SDL_Init(SDL_INIT_VIDEO < 0)) {
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
				}else {
					// Get the window's surface.
					screenSurface = SDL_GetWindowSurface(window);
				}
			}
		}
	}

	return success;
}

bool loadMedia() {
	// ...return variable.
	bool success = true;

	return success;
}

void close() {
	// Deallocate the surfaces loaded.
	/*for(int x = 0; x < KEY_PRESS_SURFACE_TOTAL; ++x) {
		SDL_FreeSurface(keyPressSurfaces[x]);
		keyPressSurfaces[x] = NULL;
	}*/

	// Deallocate texture.
	if (texture != NULL) SDL_DestroyTexture(texture);
	texture = NULL;

	// Deallocate the renderer and window.
	if (renderer != NULL) SDL_DestroyRenderer(renderer);
	if (window != NULL) SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;

	// Close the SDL subsystems.
	IMG_Quit();
	SDL_Quit();

	return;
}

SDL_Surface *loadSurface(std::string path) {
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

SDL_Texture *loadTexture(std::string path) {
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
