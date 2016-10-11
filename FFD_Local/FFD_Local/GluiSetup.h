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