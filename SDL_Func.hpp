#include <iostream>

#include "SDL2\SDL.h"
#include "SDL2\SDL_mixer.h"

#include "chip8_arch.hpp"

struct SdlObj {
	// The main window and its surface.
	SDL_Window *window = NULL;
	SDL_Surface *screenSurface = NULL;

	// Surface(s) used to load images to the main window's surface.
	SDL_Surface *currentSurface = NULL;

	// Texture variables.
	SDL_Renderer *renderer = NULL;	// Window renderer.
	SDL_Texture *texture = NULL;	// Currently displayed texture.

	// Music variable.
	Mix_Music *music = NULL;

	// Sound effect variables.
	Mix_Chunk *high_SFX = NULL;
	Mix_Chunk *med_SFX = NULL;
	Mix_Chunk *low_SFX = NULL;
	Mix_Chunk *chuu_warai = NULL;

	const int SCREEN_WIDTH = 640, SCREEN_HEIGHT = 320;
	const int FPS = 500;
	const Uint32 MS_PER_FRAME = 1000 / FPS;

	bool init();

	bool loadMedia();

	void drawScreen(Chip8Arch Chip8);

	void close();

	SDL_Surface *loadSurface(std::string path);

	SDL_Texture *loadTexture(std::string path);
};

class Timer {
private:
	Uint32 start_ticks;

	// Number of current ticks when the timer is paused.
	Uint32 paused_ticks;

	// Timer's status.
	bool paused;
	bool started;

public:
	// Constructor.
	Timer();

	void start();
	void stop();
	void pause();
	void unpause();

	// Gets the timer's number of ticks.
	Uint32 getTicks();

	// Checks the status of the timer.
	bool isStarted();
	bool isPaused();	
};