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
#ifndef mesh_h
#define mesh_h

#include <cstdlib>
#include <GL\glui.h>
//#include <GL\freeglut.h>
#include <map>
#include <Windows.h>
#include <WTypes.h>

using namespace std;



// Mesh functions
void loadMesh(std::string path);
void findMinMax(float tempX, float tempY, float tempZ);
void calcFaceNorm(void);
void calcVertNorm(void);
void setToOrigin(void);
void scaleToOrigin(void);

// Draw functions
double editLineWidth(int width);
void drawMeshWire(void);
void drawMeshSolid(void);
void drawMeshPoint(void);
void drawBbox(void);
void drawGrid(void);
void drawAxes(void);

// Dialog functions
void createFileDialog(void);
int createMsgbox(int msgboxID);
enum
{
	ERR_no_mesh = 0,
	ERR_no_pt
};

#endif