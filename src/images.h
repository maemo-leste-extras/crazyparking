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
#ifndef _IMAGES_H_
#define _IMAGES_H_

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#define WIDTH	6
#define HEIGHT	6
#define OFFSETX	171
#define OFFSETY	75
#define GRID	55
#define CARS	20
#define IDLE	1
#define SELECT	2
#define MOVE	3

#define LEVEL_OFFSETX	65
#define LEVEL_OFFSETY	120
#define LEVEL_OFFSETW	43
#define LEVEL_OFFSETH	48
#define LEVEL_OFFSETX2	LEVEL_OFFSETX + 2 * LEVEL_OFFSETW
#define LEVEL_OFFSETY2	LEVEL_OFFSETY + 6 * LEVEL_OFFSETH
#define MOVE_OFFSETX	670
#define MOVE_OFFSETY	267
#define QUIT_OFFSETX	570
#define QUIT_OFFSETY	358
#define QUIT_OFFSETX2	QUIT_OFFSETX + 70
#define QUIT_OFFSETY2	QUIT_OFFSETY + 40
#define RESET_OFFSETX	660
#define RESET_OFFSETY	358
#define RESET_OFFSETX2	RESET_OFFSETX + 105
#define RESET_OFFSETY2	RESET_OFFSETY + 32

struct CAR {
	int type, big;
	int horizontal;
	int x, y;
};

struct STATE {
	int state, car;
	int x, y, px, py;
	int cnt, over, inc;
};

struct MAZE {
	int ncars;
	struct CAR car[CARS];
};

extern int sound;
extern Mix_Chunk *scar, *struck;
extern SDL_Surface *way_up[3], *way_right[3];
extern SDL_Surface *screen, *park, *number[10];
extern SDL_Surface *car_up[CARS], *car_right[CARS];
extern SDL_Surface *car_grid_up[3], *car_grid_right[3];
extern SDL_Surface *truck_grid_up[3], *truck_grid_right[3];
extern SDL_Surface *level[12], *level_over, *finish;

int LoadImages();
void DrawScreen();
void DrawBody(int ncar, int pos, int flip);
void FreeSurfaces();

#endif //_IMAGES_H_
