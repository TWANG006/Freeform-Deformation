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
#ifndef main_h
#define main_h

#include <cstdlib>
#include <GL\glui.h>
#include <GL\glut.h>
#include <Windows.h>
#include <WTypes.h>

using namespace std;

// Camera Control
#define TRANSFORM_NONE			0
#define TRANSFORM_ROTATE		1
#define TRANSFORM_SCALE			2
#define TRANSFORM_TRANSLATE		3
#define TRANSFORM_SELECT		4
#define TRANSFORM_SELECT_LATT	5

// Camera variables
static int transformMode = 0;
static int mouseX, mouseY;

// Window
static int win;
static int winHor = 0;
static int winVer = 0;
static int glutWidth;
static int glutHeight;
static float xyAspect;
void getResolution(int &horizontal, int &vertical);

// Callback functions
void display(void);
void reshape(int width, int height);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void hotkey(unsigned char key, int x, int y);

// Select points flags
bool foundArea	= false;	// The selected area has vertices
bool foundCP	= false;	// There is a selected CP
bool selecting  = true;		// Mouse dragging for selecting area
bool selectLatt = false;	// Selecting CP to drag
bool movingLatt = false;	// Moving CP

// Select points mouse coordinates
static int botLeft[2]	  = {0, 0};
static int topRight[2]	  = {0, 0};
static int selBotLeft[2]  = {0, 0};
static int selTopRight[2] = {0, 0};

#endif