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
#include "images.h"
#include "level.h"
#include "crazypark.h"

// Global variables
int sound = 0;
Mix_Chunk *scar, *struck;
SDL_Surface *way_up[3], *way_right[3];
SDL_Surface *screen, *park, *number[10];
SDL_Surface *car_up[CARS], *car_right[CARS];
SDL_Surface *car_grid_up[3], *car_grid_right[3];
SDL_Surface *truck_grid_up[3], *truck_grid_right[3];
SDL_Surface *level[12], *level_over, *finish;

// Load image in video memory
SDL_Surface *load_image(char *filename, int transparent) {
	char filepath[256];
	SDL_Surface *img, *img2;

	// Load file image
	sprintf(filepath, "pixmaps/%s", filename);
        char *fn = crazypark_file_path(filepath);
	img = SDL_LoadBMP(fn);
        free(fn);
	if (!img) {
		fprintf(stderr, "File not found: %s\n", filepath);
		return 0;
	}
	
	// Create hardware surface
	img2 = SDL_CreateRGBSurface(SDL_HWSURFACE | (transparent ? SDL_SRCCOLORKEY :
				0), img->w, img->h, 16, 0xF800, 0x7E0, 0x1F, 0);
	if (!img2) {
		SDL_FreeSurface(img);
		fprintf(stderr, "Error creating surface!\n");
		return 0;
	}
	
	// Set color key
	if (transparent) {
		SDL_SetColorKey(img2, SDL_SRCCOLORKEY, 0xF81F);
	}
	SDL_SetAlpha(img2, 0, 0);
	
	// Copy surface
	SDL_BlitSurface(img, NULL, img2, NULL);
	SDL_FreeSurface(img);
	
	return img2;
}

// Load images
int LoadImages() {
	int i;
	char filename[128];

	// Load grids
	for (i = 0; i < 3; i++) {

		// Vertical car grid
		sprintf(filename, "over_up_%d.bmp", i + 1);
		car_grid_up[i] = load_image(filename, 1);
		if (!car_grid_up[i]) return 0;

		// Horizontal car grid
		sprintf(filename, "over_right_%d.bmp", i + 1);
		car_grid_right[i] = load_image(filename, 1);
		if (!car_grid_right[i]) return 0;

		// Vertical truck grid
		sprintf(filename, "truck_over_up_%d.bmp", i + 1);
		truck_grid_up[i] = load_image(filename, 1);
		if (!truck_grid_up[i]) return 0;

		// Horizontal truck grid
		sprintf(filename, "truck_over_right_%d.bmp", i + 1);
		truck_grid_right[i] = load_image(filename, 1);
		if (!truck_grid_right[i]) return 0;
		
		// Vertical way
		sprintf(filename, "grid_over_up_%d.bmp", i + 1);
		way_up[i] = load_image(filename, 1);
		if (!way_up[i]) return 0;
		
		// Horizontal way
		sprintf(filename, "grid_over_right_%d.bmp", i + 1);
		way_right[i] = load_image(filename, 1);
		if (!way_right[i]) return 0;
	}		

	// Load cars
  	car_up[0] = load_image("car_red_up.bmp", 1);
	car_right[0] = load_image("car_red_right.bmp", 1);
  	car_up[1] = load_image("car_yellow_up.bmp", 1);
	car_right[1] = load_image("car_yellow_right.bmp", 1);
  	car_up[2] = load_image("car_green_up.bmp", 1);
	car_right[2] = load_image("car_green_right.bmp", 1);
  	car_up[3] = load_image("car_blue_up.bmp", 1);
	car_right[3] = load_image("car_blue_right.bmp", 1);
  	car_up[4] = load_image("truck_up.bmp", 1);
	car_right[4] = load_image("truck_right.bmp", 1);
  	car_up[5] = load_image("truck_2_up.bmp", 1);
	car_right[5] = load_image("truck_2_right.bmp", 1);
  	for (i = 0; i < 6; i++) {
  		if (!car_up[i]) return 0;
  		if (!car_right[i]) return 0;
  	}

	// Load numbers
	for (i = 0; i < 10; i++) {
		sprintf(filename, "font_%d.bmp", i);
		number[i] = load_image(filename, 0);
		if (!number[i]) return 0;
	}

	// Load others images
	park = load_image("park.bmp", 0);
	if (!park) return 0;
  	level_over = load_image("level_over.bmp", 1);
  	if (!level_over) return 0;
  	finish = load_image("level_cleared.bmp", 1);
  	if (!finish) return 0;
		
	// Load success
	return 1;
}

// Draw moves score
void DrawMoves() {
	char num[6], *b = num;
	SDL_Rect dest;
	
	// Offsets
	dest.h = dest.w = 0;
	dest.x = MOVE_OFFSETX;
	dest.y = MOVE_OFFSETY;
	sprintf(num, "%d", moves);

	// Draw each number
	while (*b) {
		SDL_BlitSurface(number[*b++ - '0'], NULL, screen, &dest);
		dest.x += 20;		
	}		
}

// Draw body game
void DrawBody(int ncar, int pos, int flip) {
	int i;
	SDL_Rect dest;
	
	// Draw background
	if (ncar != -1) {
		if (car[ncar].horizontal) {
			dest.x = OFFSETX;
			dest.y = OFFSETY + car[ncar].y * GRID;
			dest.w = GRID * 6;
			dest.h = GRID;
		} else {
			dest.x = OFFSETX + car[ncar].x * GRID;
			dest.y = OFFSETY;
			dest.w = GRID;
			dest.h = GRID * 6;
		}
		SDL_BlitSurface(park, &dest, screen, &dest);
	} else SDL_BlitSurface(park, NULL, screen, NULL);
	DrawMoves();
	
	// Draw selected level
	dest.w = dest.h = 0;
	dest.x = (actual_level & 1) * LEVEL_OFFSETW + LEVEL_OFFSETX;
	dest.y = (actual_level / 2) * LEVEL_OFFSETH + LEVEL_OFFSETY;
	SDL_BlitSurface(level_over, NULL, screen, &dest);
	
	// Draw each car
	for (i = 0; i < cars; i++) {
		dest.x = (ncar == i && car[i].horizontal ? pos : car[i].x * GRID) + OFFSETX;
		dest.y = (ncar == i && !car[i].horizontal ? pos : car[i].y * GRID) + OFFSETY;
		SDL_BlitSurface(car[i].horizontal ? car_right[car[i].type] : car_up[car[i].type], NULL, screen, &dest);
	}

	// Update screen
	if (flip) {
		if (ncar == -1) SDL_UpdateRect(screen, 0, 0, 0, 0);
		else SDL_UpdateRect(screen, OFFSETX, OFFSETY, GRID * 7, GRID * 6);
	}
}

// Draw game screen
void DrawScreen() {
	int i, n, x, y;	
	SDL_Rect dest;
	struct CAR *pCar = &car[state.car];
	
	dest.w = dest.h = 0;
	switch (state.state) {

		// Idle state
		case IDLE:
			DrawBody(-1, 0, 1);
			break;
			
		// Car selected
		case SELECT:
			dest.x = pCar->x * GRID + OFFSETX;
			dest.y = pCar->y * GRID + OFFSETY;
			if (pCar->big) SDL_BlitSurface(pCar->horizontal ? truck_grid_right[state.over] : truck_grid_up[state.over], NULL, screen, &dest);
			else SDL_BlitSurface(pCar->horizontal ? car_grid_right[state.over] : car_grid_up[state.over], NULL, screen, &dest);
			if (pCar->horizontal) {
				x = pCar->x - 1;
				while (x >= 0 && !matrix[pCar->y][x]) {
					dest.x = x-- * GRID + OFFSETX;
					SDL_BlitSurface(way_right[state.over], NULL, screen, &dest);
				}
				x = pCar->x + (pCar->big ? 3 : 2);
				while (x < WIDTH && !matrix[pCar->y][x]) {
					dest.x = x++ * GRID + OFFSETX;
					SDL_BlitSurface(way_right[state.over], NULL, screen, &dest);
				}
			} else {
				y = pCar->y - 1;
				while (y >= 0 && !matrix[y][pCar->x]) {
					dest.y = y-- * GRID + OFFSETY;
					SDL_BlitSurface(way_up[state.over], NULL, screen, &dest);
				}
				y = pCar->y + (pCar->big ? 3 : 2);
				while (y < HEIGHT && !matrix[y][pCar->x]) {
					dest.y = y++ * GRID + OFFSETY;
					SDL_BlitSurface(way_up[state.over], NULL, screen, &dest);
				}
			}
			
			// Change grid
			state.cnt--;
			if (!state.cnt) {
				state.cnt = 15;
				state.over += state.inc;
				if (state.over == 0 || state.over == 2) state.inc = -state.inc;
			}			

			// Update screen
			if (pCar->horizontal) SDL_UpdateRect(screen, OFFSETX, OFFSETY + pCar->y * GRID, GRID * 6, GRID);
			else SDL_UpdateRect(screen, OFFSETX + pCar->x * GRID, OFFSETY, GRID, GRID * 6);
			break;
			
		// Move a car
		case MOVE:
			moves++;
			DrawBody(-1, 0, 1);
			n = matrix[pCar->y][pCar->x];
			if (pCar->horizontal) {
				matrix[pCar->y][pCar->x] = 0;
				matrix[pCar->y][pCar->x + 1] = 0;
				if (pCar->big) matrix[pCar->y][pCar->x + 2] = 0;
				if (state.px > state.x) {
					if (sound) Mix_PlayChannel(-1, (pCar->big ? struck : scar), 0);
					for (i = state.x * GRID; i <= state.px * GRID; i += 5) DrawBody(state.car, i, 1);
					pCar->x += state.px - state.x;
				} else {
					if (sound) Mix_PlayChannel(-1, (pCar->big ? struck : scar), 0);
					for (i = state.x * GRID; i >= state.px * GRID; i -= 5) DrawBody(state.car, i, 1);
					pCar->x -= state.x - state.px;
				}
				matrix[pCar->y][pCar->x] = n;
				matrix[pCar->y][pCar->x + 1] = n;
				if (pCar->big) matrix[pCar->y][pCar->x + 2] = n;
			} else {
				matrix[pCar->y][pCar->x] = 0;
				matrix[pCar->y + 1][pCar->x] = 0;
				if (pCar->big) matrix[pCar->y + 2][pCar->x] = 0;
				if (state.py > state.y) {
					if (sound) Mix_PlayChannel(-1, (pCar->big ? struck : scar), 0);
					for (i = state.y * GRID; i <= state.py * GRID; i += 5) DrawBody(state.car, i, 1);
					pCar->y += state.py - state.y;
				} else {
					if (sound) Mix_PlayChannel(-1, (pCar->big ? struck : scar), 0);
					for (i = state.y * GRID; i >= state.py * GRID; i -= 5) DrawBody(state.car, i, 1);
					pCar->y -= state.y - state.py;
				}
				matrix[pCar->y][pCar->x] = n;
				matrix[pCar->y + 1][pCar->x] = n;
				if (pCar->big) matrix[pCar->y + 2][pCar->x] = n;
			}
			state.state = IDLE;
			
			// End state?
			for (i = car[0].x + 2; i < WIDTH; i++) if (matrix[2][i]) break;
			if (i == WIDTH) {
				for (i = car[0].x * GRID; i < (WIDTH - 1) * GRID; i += 5) DrawBody(0, i, 1);
				car[0].x = 5;
				for (i = 480; i >= 195; i -= 15) {
					DrawBody(-1, 0, 0);
					dest.x = 198;
					dest.y = i;
					SDL_BlitSurface(finish, NULL, screen, &dest);
					SDL_UpdateRect(screen, OFFSETX, 0, GRID * 7, 480);
				}
				SDL_Delay(5000);
				actual_level++;
				if (actual_level == 12) actual_level = 0;
				Reset();
			}
			break;
	}	
}

// Free surfaces
void FreeSurfaces() {
	int i;

	// Free others images
	SDL_FreeSurface(park);
	SDL_FreeSurface(level_over);
	SDL_FreeSurface(finish);

	// Free numbers
	for (i = 0; i < 10; i++) SDL_FreeSurface(number[i]);

	// Free cars and trucks
	for (i = 0; i < 3; i++) {
		SDL_FreeSurface(car_grid_up[i]);
		SDL_FreeSurface(car_grid_right[i]);
		SDL_FreeSurface(truck_grid_up[i]);
		SDL_FreeSurface(truck_grid_right[i]);
		SDL_FreeSurface(way_up[i]);
		SDL_FreeSurface(way_right[i]);
	}

	// Free grids
	for (i = 0; i < 4; i++) {
		SDL_FreeSurface(car_up[i]);
		SDL_FreeSurface(car_right[i]);
	}
}
