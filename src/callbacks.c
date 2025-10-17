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
#include <stdio.h>
#include <hgw/hgw.h>
#include "callbacks.h"
#include "images.h"
#include "level.h"

// Pause the game
int exit_callback(int errcode) {
	FILE *han;

	// Save state
	han = fopen("/tmp/.crazyparking-save", "wb");
	if (han) {
		fwrite(&actual_level, sizeof(int), 1, han);
		fwrite(&moves, sizeof(int), 1, han);
		fwrite(car, sizeof(struct CAR), cars, han);
		fclose(han);
	}

	return 0;
}

// Quit game
int quit_callback(int errcode) {
	remove("/tmp/.crazyparking-save");

	hgw_context_compat_destroy_quit(hgw_context);
	return 0;
}

int flush_callback(int errcode) {
	remove("/tmp/.crazyparking-save");

	return 0;
}
