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
extern HgwContext *hgw_context;

int exit_callback(int errcode/*, AppData *app_data*/);
int quit_callback(int errcode/*, AppData *app_data*/);
int flush_callback(int errcode);
