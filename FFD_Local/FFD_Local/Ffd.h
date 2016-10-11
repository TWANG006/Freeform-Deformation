#ifndef ffd_h
#define ffd_h

#include <GL\glui.h>
#include <GL\glut.h>
#include <iostream>

using namespace std;

// Structure
struct Vertex
{
	GLdouble x, y, z;
	bool selected;
};

// Math functions
Vertex subtraction(Vertex u, Vertex v);
Vertex crossProduct(Vertex u, Vertex v);
double dotProduct(Vertex u, Vertex v);
double factorial(int f);

// FFD calculation functions
double bernsteinPoly(int bottom, int top, double stu);
Vertex trivariateBernstein(Vertex vert);
Vertex convertToSTU(Vertex vert);

// FFD initialisation functions
void initFFD(double maxX, double maxY, double maxZ, double minX, double minY, double minZ);
void initAxes(void);
void initPlanes(void);
void initControlLattice(void);
void resetAxes(void);

// Selection functions
void drawSelectedArea(int botLeft[2], int topRight[2]);
bool getSelectedArea(int botLeft[2], int topRight[2]);
bool checkSelectedArea(Vertex pt, int botLeft[2], int topRight[2]);
void findSelectedMinMax(float tempX, float tempY, float tempZ);

// Lattice functions
void resetLattice(void);
void drawLattice(void);
bool getSelectedCP(int startX, int startY);
bool checkSelectedCP(Vertex pt, int x, int y);
void moveSelectedCP(int endX, int endY);
void deformMesh(void);

#endif