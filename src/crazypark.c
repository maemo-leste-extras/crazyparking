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
#include <gtk/gtk.h>
#include <gconf/gconf.h>
#include <gconf/gconf-client.h>

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <hgw/hgw.h>
#include <unistd.h>
#include "images.h"
#include "level.h"
#include "callbacks.h"

#define SETTINGS_LEVEL "/apps/osso/crazyparking/level"
#define SETTINGS_SOUND "/apps/osso/crazyparking/sound"

HgwContext *hgw_context = NULL;


// Main function
int crazy_main() {
	FILE *han;
	SDL_Event event;
	int b_up = 1, x, y, i, done = 0;
	int enable_sound, level;
	int has_focus = 1;
	Uint32 timeout = 10000;
	

    GConfClient *gc_client = NULL;
    g_type_init();
    gc_client = gconf_client_get_default();
    level = gconf_client_get_int(gc_client, SETTINGS_LEVEL, NULL);
    //enable_sound = gconf_client_get_int(gc_client, SETTINGS_SOUND, NULL);
    enable_sound = 0;


	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
	    fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
    	return 0;
  	}

	// Initialize audio
  	if (enable_sound && !Mix_OpenAudio(22050, (Uint16)AUDIO_U8, 2, 256)) 
        sound = 1;

	// Initialize video
	screen = SDL_SetVideoMode(800, 480, 16, SDL_SWSURFACE | SDL_FULLSCREEN);
	if (screen == NULL) {
		fprintf(stderr, "Unable to set 800x480 video: %s\n", SDL_GetError());
		return 0;
	}
	SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_SetCaption("CrazyParking", NULL);

	// Load images
	if (!LoadImages()) {
		fprintf(stderr, "Couldn't load images!\n");
		return 0;
	}

	// Load sounds
  	if (sound) {
	  	scar = Mix_LoadWAV(SOUNDPREFIX "/car.wav");
	  	struck = Mix_LoadWAV(SOUNDPREFIX "/truck.wav");
  	}

	// Initialize game
    actual_level = level -1;
	Reset();
	// Load state
	han = fopen("/tmp/.crazyparking-save", "rb");
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

			// Window focus change
			case SDL_ACTIVEEVENT:
				if (event.active.gain == 0) {
					done = 1;
					exit_callback(0);
				}
				break;

			// Button released
			case SDL_MOUSEBUTTONUP:
				b_up = 1;
				break;

			// Button pressed
			case SDL_MOUSEBUTTONDOWN:
				has_focus = 1;
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
    		//hgw_msg_compat_receive(hgw_context, 0);

#if HGW_FUNC
		HgwMessage hgw_message;
		if ( !hgw_msg_check_incoming(hgw_context, &hgw_message, HGW_MSG_FLAG_INVOKE_CB) ) {
			if (hgw_message.type == HGW_MSG_TYPE_DEVSTATE && (hgw_message.e_val & HGW_DEVICE_STATE_DISPLAYOFF)) {
				has_focus = 1;
			} 
			if (hgw_message.type == HGW_MSG_TYPE_DEVSTATE 
			&& (hgw_message.e_val & (HGW_DEVICE_STATE_DISPLAYOFF))
			&& (hgw_message.e_val & (HGW_DEVICE_STATE_ON))) {
				has_focus = 0;
			} 
		}
		hgw_msg_free_data(&hgw_message);
#endif

		// Timeout
		if (has_focus) timeout = SDL_GetTicks() + 10000;
		if (SDL_GetTicks() > timeout) {
			done = 1;
			exit_callback(0);
		}
	}

	// Shut down SDL
	if (sound) {
		Mix_FreeChunk(scar);
		Mix_FreeChunk(struck);
		Mix_CloseAudio();
	}

    gconf_client_set_int(gc_client, SETTINGS_LEVEL, actual_level, NULL);
	FreeSurfaces();
	SDL_Quit();
	return 0;
}


// Initialize libshadow
int main(int argc, char **argv) {

#if HGW_FUNC
	hgw_context = hgw_context_compat_init(argc, argv);
	if (!hgw_context) {
		fprintf(stderr, "Cannot init hildon-games-startup!\n");
		return 0;
	}
	hgw_compat_set_cb_exit(hgw_context, exit_callback);
	hgw_compat_set_cb_quit(hgw_context, quit_callback);
	hgw_compat_set_cb_flush(hgw_context, flush_callback);
    if(!hgw_context_compat_check(hgw_context)) return 0;

	/* hildon-games-wrapper part */
    	hgw_msg_compat_receive(hgw_context, 0);
	usleep(100);
#endif
	// Main game
	crazy_main();

#if HGW_FUNC
	hgw_context_compat_destroy_deinit(hgw_context);
#endif

	return 0;
}
