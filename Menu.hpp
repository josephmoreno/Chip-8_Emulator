#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <dirent.h>
#include <cctype>
#include <SDL2/SDL.h>

#include "SDL_Func.hpp"

struct Menu {
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* fontsheet = nullptr;
    SDL_Texture* arrow = nullptr;
    SDL_Event* e = nullptr;
    bool* quit = nullptr;

    std::vector<std::tuple<std::string, SDL_Rect>> rom_lib; // ROM title and destination rect for mouse events
    std::string rom_lib_path;
    std::string cur_rom = "";

    const int titles_per_page = SCREEN_HEIGHT / 32;   // Font rendered to screen will be 32 pixels in height
    int num_of_pages = 0;
    int page = 0;

    Uint8 highlight_alpha = 0;
    Uint8 blink_alpha = 0;
    int mouse_x = -1;
    int mouse_y = -1;

    Menu(SDL_Renderer& renderer, SDL_Texture& fontsheet, SDL_Texture& arrow, SDL_Event& e, bool& quit) : renderer(&renderer), fontsheet(&fontsheet), arrow(&arrow), e(&e), quit(&quit) {};
    ~Menu() { if (fontsheet != nullptr) SDL_DestroyTexture(fontsheet); if (arrow != nullptr) SDL_DestroyTexture(arrow); };

    void initRomLib(const char* path) {
        rom_lib_path = path;

        // Open directory and read file names (dirent.h)
        DIR *dir;
        struct dirent *d;
        int count = 0;

        dir = opendir(path);

        if (dir) {
            while ((d = readdir(dir)) != NULL) {
                if (strcmp(d->d_name, ".") != 0 && strcmp(d->d_name, "..") != 0) {
                    rom_lib.push_back(std::tuple<std::string, SDL_Rect>(d->d_name, {0, (count % titles_per_page) * 32, static_cast<int>(32 * strlen(d->d_name)), 32}));
                    ++count;
                }
            }
        
            closedir(dir);	
        }

        num_of_pages = rom_lib.size() / titles_per_page + (rom_lib.size() % titles_per_page > 0 ? 1 : 0);
    };

    void printRomLib() {
        std::cout << std::endl;

        for(std::vector<std::tuple<std::string, SDL_Rect>>::iterator it = rom_lib.begin(); it < rom_lib.end(); ++it)
            std::cout << std::get<0>(*it) << std::endl;

        std::cout << std:: endl;
    };

    void handleEvents() {
        // Take input from the player.
        while (SDL_PollEvent(e) != 0) {

            // Top-right exit button...
            if (e->type == SDL_QUIT) {
                *quit = true;

            // Key pressed...
            }else if (e->type == SDL_KEYDOWN) {
                switch(e->key.keysym.sym) {
                    case SDLK_ESCAPE:
                        *quit = true;
                        break;

                    default:
                        break;
                }
            }else if (e->type == SDL_MOUSEBUTTONUP) {
                int selection = mouseSelection(e->button.x, e->button.y);

                if (selection != -3) {
                    switch(e->button.button) {
                        case 1: // Left-click
                            // std::cout << selection << std::endl;
                            
                            if (selection == -1)
                                ++page;
                            else if (selection == -2)
                                --page;
                            else
                                cur_rom = std::get<0>(rom_lib[selection]);
                            
                            break;

                        default:
                            break;
                    }
                }
            }else if (e->type == SDL_MOUSEMOTION) {
                mouse_x = e->motion.x;
                mouse_y = e->motion.y;
            }
        }
    };

    void update() {
        // Going to put "breathing" highlight on the title the cursor is hovering over
        int hl_frame = (SDL_GetTicks() / 100) % 20;
        highlight_alpha = (hl_frame <= 10 ? hl_frame : 20 - hl_frame) * 25;

        // Going to blink menu arrows when cursor is hovering over
        blink_alpha = ((SDL_GetTicks() / 250) % 2) * 255;
    };
    
    void render() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        SDL_Rect src, dest;

        // For each ROM title
        for(int i = 0; i < static_cast<int>(page == num_of_pages - 1 ? rom_lib.size() % titles_per_page : titles_per_page); ++i) {
            std::string title = std::get<0>(rom_lib[page * titles_per_page + i]);

            // For each character in the title
            for(int j = 0; j < static_cast<int>(title.length()); ++j) {
                unsigned char c = tolower(title[j]);

                // Get the character from the font sheet and render it to the screen
                if (c >= 'a' && c <= 'z')
                    src = {(c - 'a') * 32, 0, 32, 32};
                else if (c >= '0' && c <= '9')
                    src = {((c - '0') + 26) * 32, 0, 32, 32};

                dest = {j * 32, i * 32, 32, 32};

                SDL_RenderCopyEx(renderer, fontsheet, &src, &dest, 0, NULL, SDL_FLIP_NONE);
            }
        }

        // Render down arrow if there's a next page of titles
        if (page < num_of_pages - 1) {
            src = {0, 0, 32, 32};
            dest = {SCREEN_WIDTH - 32, SCREEN_HEIGHT - 32, 32, 32};
            SDL_RenderCopyEx(renderer, arrow, &src, &dest, 0, NULL, SDL_FLIP_VERTICAL);
        }

        // Render up arrow if there's a previous page of titles
        if (page > 0) {
            src = {0, 0, 32, 32};
            dest = {SCREEN_WIDTH - 32, 0, 32, 32};
            SDL_RenderCopyEx(renderer, arrow, &src, &dest, 0, NULL, SDL_FLIP_NONE);
        }

        int selection = mouseSelection(mouse_x, mouse_y);

        if (selection >= 0)
            drawHighlight(255, 255, 255, std::get<1>(rom_lib[selection]), highlight_alpha);
        else if (selection == -1)
            drawHighlight(0, 0, 0, {SCREEN_WIDTH - 32, SCREEN_HEIGHT - 32, 32, 32}, blink_alpha);
        else if (selection == -2)
            drawHighlight(0, 0, 0, {SCREEN_WIDTH - 32, 0, 32, 32}, blink_alpha);

        SDL_RenderPresent(renderer);
    };

    int mouseSelection(int x, int y) {
        int selection = -3; // -1 = next page button, -2 = previous page button, -3 = no selection

        // Check if the ROM titles are being clicked
        for(int i = 0; i < static_cast<int>(page == num_of_pages - 1 ? rom_lib.size() % titles_per_page : titles_per_page); ++i) {
            int cur = page * titles_per_page + i;
            SDL_Rect rect = std::get<1>(rom_lib[cur]);
            
            if ((x >= rect.x) && (x < rect.x + rect.w) && (y >= rect.y) && (y < rect.y + rect.h)) {
                selection = cur;
                break;
            }
        }

        // If still no selection after checking titles, check next and previous page buttons
        if (selection <= -3) {
            if ((x >= SCREEN_WIDTH - 32) && (x < SCREEN_WIDTH) && (y >= SCREEN_HEIGHT - 32) && (y < SCREEN_HEIGHT) && (page < num_of_pages - 1))
                selection = -1;
            else if ((x >= SCREEN_WIDTH - 32) && (x < SCREEN_WIDTH) && (y >= 0) && (y < 32) && (page > 0))
                selection = -2;
        }

        return(selection);
    };

    void drawHighlight(int r, int g, int b, SDL_Rect dest, Uint8 alpha) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, r, g, b, alpha);
        SDL_RenderFillRect(renderer, &dest);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    };
};