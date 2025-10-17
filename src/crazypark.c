/*
* This file is part of Crazy Parking
*
* Copyright (C) 2006 INdT - Instituto Nokia de Tecnologia
* http://www.indt.org/maemo
*
* This software is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* as published by the Free Software Foundation; either version 2.1 of
* the License, or (at your option) any later version.
*
* This software is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this software; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
* 02110-1301 USA
*
*/
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <limits.h>
#include "images.h"
#include "level.h"
#include "crazypark.h"
#include "config.h"

char *
crazypark_file_path(const char *filename)
{
    char *result = NULL;

    int res = asprintf(&result, "%s/%s", CRAZYPARKING_DATADIR, filename);
    if (res == -1) {
        fprintf(stderr, "Cannot allocate string buffer\n");
        exit(1);
    }

    struct stat st;
    if (stat(result, &st) != 0) {
        // File does not exist; try local path
        free(result);
        res = asprintf(&result, "data/%s", filename);
        if (res == -1) {
            fprintf(stderr, "Cannot allocate string buffer\n");
            exit(1);
        }
    }

    return result;
}

static char
crazypark_save_file_path[PATH_MAX];

// Pause the game
int exit_callback(int errcode) {
	FILE *han;

	// Save state
	han = fopen(crazypark_save_file_path, "wb");
	if (han) {
		fwrite(&actual_level, sizeof(int), 1, han);
		fwrite(&moves, sizeof(int), 1, han);
		fwrite(car, sizeof(struct CAR), cars, han);
		fclose(han);
	}

	return 0;
}

// Main function
int main(int argc, char **argv) {
	FILE *han;
	SDL_Event event;
	int b_up = 1, x, y, i, done = 0;
	int enable_sound, level, fullscreen;

        (void)snprintf(crazypark_save_file_path, sizeof(crazypark_save_file_path),
                "%s/.crazyparking-save", getenv("HOME") ?: "/tmp");

        // TODO: Make those settings configurable
        level = 1;
        enable_sound = 1;
        fullscreen = 1;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
	    fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
    	return 0;
  	}

	// Initialize audio
  	if (enable_sound && !Mix_OpenAudio(22050, (Uint16)AUDIO_U8, 2, 256)) 
        sound = 1;

	// Initialize video
	screen = SDL_SetVideoMode(800, 480, 16, SDL_SWSURFACE | (fullscreen ? SDL_FULLSCREEN : 0));
	if (screen == NULL) {
		fprintf(stderr, "Unable to set 800x480 video: %s\n", SDL_GetError());
		return 0;
	}
	//SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_SetCaption("CrazyParking", NULL);

	// Load images
	if (!LoadImages()) {
		fprintf(stderr, "Couldn't load images!\n");
		return 0;
	}

	// Load sounds
  	if (sound) {
                char *fn = crazypark_file_path("sounds/car.wav");
	  	scar = Mix_LoadWAV(fn);
                free(fn);

                fn = crazypark_file_path("sounds/truck.wav");
	  	struck = Mix_LoadWAV(fn);
                free(fn);
  	}

	// Initialize game
    actual_level = level -1;
	Reset();
	// Load state
	han = fopen(crazypark_save_file_path, "rb");
	if (han) {
		fread(&actual_level, sizeof(int), 1, han);
		fread(&moves, sizeof(int), 1, han);
		fread(car, sizeof(struct CAR), maze[actual_level].ncars, han);
		fclose(han);
//    	actual_level = level -1;
		LoadMaze(actual_level, 1);
	}
	// Draw first screen
	DrawScreen();
	SDL_UpdateRect(screen, 0, 0, 0, 0);

	// Main loop
	while (!done) {
		SDL_PollEvent(&event);
		if (event.key.state == SDL_PRESSED) {
		       if (event.key.keysym.sym == SDLK_ESCAPE) {
			       done = 1;
			       exit_callback(0);
		       } else if (event.key.keysym.sym == SDLK_F4 ||
                          event.key.keysym.sym == SDLK_F5 ||
                          event.key.keysym.sym == SDLK_F6) {
			       done = 1;
			       exit_callback(0);
		       }
		}

		switch (event.type) {

			// Quit the game
			case SDL_QUIT:
				done = 1;
				exit_callback(0);
				break;

			// Button released
			case SDL_MOUSEBUTTONUP:
				b_up = 1;
				break;

			// Button pressed
			case SDL_MOUSEBUTTONDOWN:
				if (!b_up) break;
				else b_up = 0;

				// Car area
				if (event.button.x >= OFFSETX &&
                    event.button.x < GRID * WIDTH + OFFSETX &&
					event.button.y >= OFFSETY &&
                    event.button.y < GRID * HEIGHT + OFFSETY) {

					// Coordinates 
					x = (event.button.x - OFFSETX) / GRID;
					y = (event.button.y - OFFSETY) / GRID;
					if (matrix[y][x]) {
						if (y > 0 && matrix[y - 1][x] == matrix[y][x]) y--;
                        if (y > 0 && matrix[y - 1][x] == matrix[y][x]) y--;
						if (x > 0 && matrix[y][x - 1] == matrix[y][x]) x--;
                        if (x > 0 && matrix[y][x - 1] == matrix[y][x]) x--;
					}

					// Select car
					if (matrix[y][x] && (state.state == IDLE ||
                                         (state.state == SELECT &&
                                          (state.x != x || state.y != y))))
                    {
						DrawBody(-1, 0, 1);
						state.state = SELECT;
						state.car = matrix[y][x] - 1;
						state.x = x; state.y = y;
						state.cnt = 15;
						state.over = 0;
						state.inc = 1;

					// Remove selection
					} else if (state.state == SELECT && state.x == x && state.y == y)
                        state.state = IDLE;

					// Validate selection
					else if (state.state == SELECT && !matrix[y][x]) {
						if (car[state.car].horizontal && car[state.car].y == y) {
							if (x < state.x) {
								for (i = x; i < state.x; i++)
                                    if (matrix[y][i])
                                        break;
								if (i == state.x) {
									state.state = MOVE;
									state.px = x;
									state.py = y;
								}
							} else {
								for (i = state.x + (car[state.car].big ? 3 : 2); i <= x; i++)
                                    if (matrix[y][i])
                                        break;
								if (i == x + 1) {
									state.state = MOVE;
									state.px = x - (car[state.car].big ? 2 : 1);
									state.py = y;
								}
							}
						} else if (!car[state.car].horizontal && car[state.car].x == x) {
							if (y < state.y) {
								for (i = y; i < state.y; i++)
                                    if (matrix[i][x])
                                        break;
								if (i == state.y) {
									state.state = MOVE;
									state.px = x;
									state.py = y;
								}
							} else {
								for (i = state.y + (car[state.car].big ? 3 : 2); i <= y; i++)
                                    if (matrix[i][x])
                                        break;
								if (i == y + 1) {
									state.state = MOVE;
									state.px = x;
									state.py = y - (car[state.car].big ? 2 : 1);
								}
							}
						}
					}

				// Quit
				} else if (event.button.x >= QUIT_OFFSETX &&
                           event.button.x < QUIT_OFFSETX2 &&
                           event.button.y >= QUIT_OFFSETY &&
                           event.button.y < QUIT_OFFSETY2)
                {
					done = 1;
					exit_callback(0);


				// Reset
				} else if (event.button.x >= RESET_OFFSETX &&
                           event.button.x < RESET_OFFSETX2 &&
                           event.button.y >= RESET_OFFSETY &&
                           event.button.y < RESET_OFFSETY2)
                {
					Reset();

				// Change level
				} else if (event.button.x >= LEVEL_OFFSETX &&
                           event.button.x < LEVEL_OFFSETX2 &&
                           event.button.y >= LEVEL_OFFSETY &&
                           event.button.y < LEVEL_OFFSETY2)
                {
					x = (event.button.x - LEVEL_OFFSETX) / LEVEL_OFFSETW;
					y = (event.button.y - LEVEL_OFFSETY) / LEVEL_OFFSETH;
					actual_level = 2 * y + x;
					Reset();
				}
				break;

			// Save CPU
			//default:
				//continue;
		}

		SDL_Delay(10);

		// Make scene
		DrawScreen();
	}

	// Shut down SDL
	if (sound) {
		Mix_FreeChunk(scar);
		Mix_FreeChunk(struck);
		Mix_CloseAudio();
	}

        // TODO: save actual_level -> SETTINGS_LEVEL
	FreeSurfaces();
	SDL_Quit();
	return 0;
}
