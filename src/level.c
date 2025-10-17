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
#include "images.h"

struct CAR car[CARS];
struct STATE state;
struct MAZE maze[32];
char matrix[HEIGHT][WIDTH];
int cars, actual_level = 0, moves;

// Load a maze selected
void LoadMaze(int nmaze, int load_state) {
	int i;
	struct CAR *pCar;
	
	// Copy cars
	cars = maze[nmaze].ncars;
	if (!load_state) memcpy(car, maze[nmaze].car, sizeof(struct CAR) * cars);

	// Fill matrix
	memset(matrix, 0, WIDTH * HEIGHT);
	for (i = 0; i < cars; i++) {
		pCar = &car[i];
		if (pCar->horizontal) {
			matrix[pCar->y][pCar->x] = i + 1;
			matrix[pCar->y][pCar->x + 1] = i + 1;
			if (pCar->big) matrix[pCar->y][pCar->x + 2] = i + 1;
		} else {
			matrix[pCar->y][pCar->x] = i + 1;
			matrix[pCar->y + 1][pCar->x] = i + 1;
			if (pCar->big) matrix[pCar->y + 2][pCar->x] = i + 1;
		}
	}
}

// Read maze file
void ReadMaze() {
	int i, j;
	FILE *han;
	int mazes, cars;
	int big, horizontal, x, y;
		
	// Open maze file
	han = fopen(MAZEPREFIX "/mazes.txt", "rt");
	
	// Read file
	srand(time(NULL));
	fscanf(han, "mazes=%d\n", &mazes);
	for (i = 0; i < mazes; i++) {
		fscanf(han, "cars=%d\n", &cars);
		maze[i].ncars = cars;
		for (j = 0; j < cars; j++) {
			fscanf(han, "car=%d,%d,%d,%d\n", &big, &horizontal, &x, &y);
			maze[i].car[j].type = (!j ? 0 : (big ? (rand() & 1) + 4 : (rand() % 3) + 1));
			maze[i].car[j].big = big;
			maze[i].car[j].horizontal = horizontal;
			maze[i].car[j].x = x;
			maze[i].car[j].y = y;
		}
	}

	// Close file
	fclose(han);
}

// Reset game
void Reset() {
	moves = 0;
	state.state = IDLE;
	ReadMaze();
	LoadMaze(actual_level, 0);
}
