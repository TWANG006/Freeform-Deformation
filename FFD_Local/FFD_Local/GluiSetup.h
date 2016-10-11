/*********************************************************************
 * Copyright © 2012-2016,
 * Elysia Ong Wei Xi,
 * WANG Tianyi: <tianyiwang666@gmail.com>
 *
 * This application (FFD) is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 ********************************************************************/

#ifndef gluisetup_h
#define gluisetup_h

#include <cstdlib>
#include <GL\glui.h>
//#include <GL\freeglut.h>

using namespace std;

// GLUI functions
void setupGlui(int win);
void setupBotWin(int win);
void setupLeftWin(int win);
void setupRightWin(int win);
void gluiCallback(int gluiControl);

// Mesh control functions
void clearMesh(void);
void clearGlui(void);
void clearView(void);
void reset(void);

#endif