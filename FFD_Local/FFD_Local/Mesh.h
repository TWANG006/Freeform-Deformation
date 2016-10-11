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