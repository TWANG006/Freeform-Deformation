#include "Ffd.h"
#include "HalfEdge.h"
#include "Mesh.h"

// Local axes variables
Vertex stuOrigin;			// Origin of STU
Vertex stuAxes;				// Magnitude of STU
Vertex axisS, axisT, axisU;	// Vectors of STU
Vertex maximum, minimum;	// Max and min vertices

// Control point variables
int l = 1;	// Plane(s) in S direction
int m = 1;	// Plane(s) in T direction
int n = 1;	// Plane(s) in U direction
Vertex lattice[5][5][5];
Vertex lattice2d[5][5][5];

// Select variables
Vertex *copyVertices;		// Copy of the vertices
Vertex *convertedVertices;	// Selected vertices on 2D projection
int startX = 0, startY = 0;	// Mouse down to select lattice point

// Lattice variables
int sel_i = 0, sel_j = 0, sel_k = 0;
bool selMinMaxFlag = false;
float selectedMaxX = 0, selectedMaxY = 0, selectedMaxZ = 0;
float selectedMinX = 0, selectedMinY = 0, selectedMinZ = 0;

// Convert variables
GLint viewport[4];
GLdouble modelView[16];
GLdouble projection[16];

// Get mesh variables
extern int numOfVert;
extern struct HE_vert *vertices, *v0;

// Get GLUI IDs
extern int ID_L, ID_M, ID_N;

// Get GLUI variables
extern GLUI_Spinner *lattLSpinner, *lattMSpinner, *lattNSpinner;

Vertex subtraction(Vertex u, Vertex v)
{
	/* Create temp vertex */
	Vertex subtract;
	subtract.x = 0;
	subtract.y = 0;
	subtract.z = 0;

	/* Compute subtraction */
	subtract.x = u.x - v.x;
	subtract.y = u.y - v.y;
	subtract.z = u.z - v.z;
	return subtract;
}

Vertex crossProduct(Vertex u, Vertex v)
{
	/* Create temp vertex */
	Vertex crossPdt;
	crossPdt.x = 0;
	crossPdt.y = 0;
	crossPdt.z = 0;

	/* Compute cross product */
	crossPdt.x = u.y*v.z - u.z*v.y;
	crossPdt.y = u.z*v.x - u.x*v.z;
	crossPdt.z = u.x*v.y - u.y*v.x;
	return crossPdt;
}

double dotProduct(Vertex u, Vertex v)
{
	double dotPdt = 0.0;
	dotPdt = u.x*v.x + u.y*v.y + u.z*v.z;
	return dotPdt;
}

double factorial(int f)
{
	if(f <= 1)
		return 1;
	else
		return f * factorial(f-1);
}

double bernsteinPoly(int numer, int denom, double stu)
{
	double binomCoeff = 0.0;
	double bernstein  = 0.0;
	binomCoeff = (double) (factorial(numer)) / (double) ((factorial(denom)*factorial(numer-denom)));
	bernstein = binomCoeff * pow((1-stu),(numer-denom)) * pow(stu,denom);
	return bernstein;
}

Vertex trivariateBernstein(Vertex vert)
{
	Vertex stuVert;
	stuVert.x = 0;
	stuVert.y = 0;
	stuVert.z = 0;
	stuVert = convertToSTU(vert);
	/*cout << "Vertex (XYZ): " << vert.x << ", " << vert.y << ", " << vert.z << "\n";
	cout << "Vertex (STU): " << stuVert.x << ", " << stuVert.y << ", " << stuVert.z << "\n";*/
	
	double weight = 0.0;
	Vertex convertedVert;
	convertedVert.x = 0;
	convertedVert.y = 0;
	convertedVert.z = 0;

	/* Performing summations using for loops */
	for(int i = 0; i <= l; i++) {
		for(int j = 0; j <= m; j++) {
			for(int k = 0; k <= n; k++) {
				weight = 0;
				weight = bernsteinPoly(n, k, stuVert.z) * bernsteinPoly(m, j, stuVert.y) * bernsteinPoly(l, i, stuVert.x);
				convertedVert.x += weight * lattice[i][j][k].x;
				convertedVert.y += weight * lattice[i][j][k].y;
				convertedVert.z += weight * lattice[i][j][k].z;
			}
		}
	}
	return convertedVert;
}

Vertex convertToSTU(Vertex vert)
{
	Vertex vertexSTU;
	vertexSTU.x = 0;
	vertexSTU.y = 0;
	vertexSTU.z = 0;

	Vertex TxU = crossProduct(axisT, axisU);
	Vertex SxU = crossProduct(axisS, axisU);
	Vertex SxT = crossProduct(axisS, axisT);
	Vertex diff = subtraction(vert, stuOrigin);

	vertexSTU.x = dotProduct(TxU,diff)/dotProduct(TxU,axisS);
	vertexSTU.y = dotProduct(SxU,diff)/dotProduct(SxU,axisT);
	vertexSTU.z = dotProduct(SxT,diff)/dotProduct(SxT,axisU);
	return vertexSTU;
}

void initFFD(double maxX, double maxY, double maxZ, double minX, double minY, double minZ)
{
	resetAxes();
	maximum.x = maxX; maximum.y = maxY; maximum.z = maxZ;
	minimum.x = minX; minimum.y = minY; minimum.z = minZ;
	initAxes();
	initPlanes();
	initControlLattice();
}

void initAxes()
{
	/* Origin of STU is min selected vertices */
	stuOrigin.x = minimum.x;
	stuOrigin.y = minimum.y;
	stuOrigin.z = minimum.z;
	cout << "STU origin: " << stuOrigin.x << ", " << stuOrigin.y << ", " << stuOrigin.z << "\n";

	/* Magnitude of STU is difference between max and min selected vertices */
	stuAxes.x = maximum.x-minimum.x;
	stuAxes.y = maximum.y-minimum.y;
	stuAxes.z = maximum.z-minimum.z;
	cout << "STU axes: " << stuAxes.x << ", " << stuAxes.y << ", " << stuAxes.z << "\n";

	axisS.x = stuAxes.x; axisS.y = axisS.z = 0;
	axisT.y = stuAxes.y; axisT.x = axisT.z = 0;
	axisU.z = stuAxes.z; axisU.x = axisU.y = 0;
}

void initPlanes()
{
	l = lattLSpinner->get_int_val();
	m = lattMSpinner->get_int_val();
	n = lattNSpinner->get_int_val();
}

void initControlLattice()
{
	for(int i = 0; i <= l; i++) {
		for(int j = 0; j <= m; j++) {
			for(int k = 0; k <= n; k++) {
				lattice[i][j][k].x = lattice[i][j][k].y = lattice[i][j][k].z = 0;
				lattice[i][j][k].selected = false;
				lattice[i][j][k].x = stuOrigin.x + ((double)i/l)*stuAxes.x;
				lattice[i][j][k].y = stuOrigin.y + ((double)j/m)*stuAxes.y;
				lattice[i][j][k].z = stuOrigin.z + ((double)k/n)*stuAxes.z;
				/*cout << "Lattice[" << i << "][" << j << "][" << k << "]: "
					 << lattice[i][j][k].x << ", " << lattice[i][j][k].y << ", " << lattice[i][j][k].z << "\n";*/
			}
		}
	}
}

void resetLattice()
{
	selMinMaxFlag = false;
	selectedMaxX = 0;
	selectedMaxY = 0;
	selectedMaxZ = 0;
	selectedMinX = 0;
	selectedMinY = 0;
	selectedMinZ = 0;
}

void resetAxes()
{
	stuOrigin.x = 0;
	stuOrigin.y = 0;
	stuOrigin.z = 0;
	stuAxes.x = 0;
	stuAxes.y = 0;
	stuAxes.z = 0;
	axisS.x = axisS.y = axisS.z = 0;
	axisT.x = axisT.y = axisT.z = 0;
	axisU.x = axisU.y = axisU.z = 0;
}

void drawLattice()
{
	/* Draw lattice lines */
	glColor3f(1, 1, 0.5);
	for(int i = 0; i <= l; i++) {
		for(int j = 0; j <= m; j++) {
			for(int k = 0; k < n; k++) {
			glBegin(GL_LINES);
				glVertex3f(lattice[i][j][k].x, lattice[i][j][k].y, lattice[i][j][k].z);
				glVertex3f(lattice[i][j][k+1].x, lattice[i][j][k+1].y, lattice[i][j][k+1].z);
			glEnd();
			}
		}
	}
	for(int i = 0; i <= l; i++) {
		for(int j = 0; j < m; j++) {
			for(int k = 0; k <= n; k++) {
			glBegin(GL_LINES);
				glVertex3f(lattice[i][j][k].x, lattice[i][j][k].y, lattice[i][j][k].z);
				glVertex3f(lattice[i][j+1][k].x, lattice[i][j+1][k].y, lattice[i][j+1][k].z);
			glEnd();
			}
		}
	}
	for(int i = 0; i < l; i++) {
		for(int j = 0; j <= m; j++) {
			for(int k = 0; k <= n; k++) {
			glBegin(GL_LINES);
				glVertex3f(lattice[i][j][k].x, lattice[i][j][k].y, lattice[i][j][k].z);
				glVertex3f(lattice[i+1][j][k].x, lattice[i+1][j][k].y, lattice[i+1][j][k].z);
			glEnd();
			}
		}
	}
	
	/* Draw lattice vertices */
	glColor3f(1, 0, 0);
	for(int i = 0; i <= l; i++) {
		for(int j = 0; j <= m; j++) {
			for(int k = 0; k <= n; k++) {
				if(lattice[i][j][k].selected)
					glColor3f(1, 0, 0);
				else glColor3f(1, 1, 0);
				glPushMatrix();
					glTranslatef(lattice[i][j][k].x, lattice[i][j][k].y, lattice[i][j][k].z);
					glutSolidSphere(0.1, 20, 20);
				glPopMatrix();
			}
		}
	}
}

void drawSelectedArea(int botLeft[2], int topRight[2])
{
	/* For 2D projection */
	int width  = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, width, 0, height);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_LIGHTING);
			glLoadIdentity();

			/* Draw translucent rectangle */
			glColor4f(1, 0.713, 0.757, 0.5);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glRectf(botLeft[0], botLeft[1], topRight[0], topRight[1]);

			/* Draw dotted outline */
			glEnable(GL_LINE_STIPPLE);
			glColor4f(0.5, 0, 1, 1.0);
			glLineStipple(3, 0xAAAA);
			glBegin(GL_LINE_LOOP);
				glVertex2f(botLeft[0], botLeft[1]);
				glVertex2f(topRight[0], botLeft[1]);
				glVertex2f(topRight[0], topRight[1]);
				glVertex2f(botLeft[0], topRight[1]);
			glEnd();
		glPopMatrix();
	glPopMatrix();
	glDisable(GL_LINE_STIPPLE);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}

bool getSelectedArea(int botLeft[2], int topRight[2])
{
	resetLattice();			// Reset selected min max with every new selected area
	bool result = false;
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	convertedVertices = new Vertex[numOfVert]();
	for(int count = 0; count < numOfVert; count++) {
		vertices[count].selected = false;	// Initialise all vertices as not selected
		
		/* Convert to 2D to check if vertices are within selected area */
		gluProject(vertices[count].x, vertices[count].y, vertices[count].z,
				   modelView, projection, viewport,
				   &convertedVertices[count].x, &convertedVertices[count].y, &convertedVertices[count].z);
		vertices[count].selected = checkSelectedArea(convertedVertices[count], botLeft, topRight);

		/* Process selected vertices */
		if(vertices[count].selected) {
			//cout << "Vertex selected: %f %f %f\n", vertices[count].x, vertices[count].y, vertices[count].z);
			findSelectedMinMax(vertices[count].x, vertices[count].y, vertices[count].z);
			result = true;
		}
	}

	/* Initialise lattice */
	initFFD(selectedMaxX, selectedMaxY, selectedMaxZ, selectedMinX, selectedMinY, selectedMinZ);
	return result;
}

bool checkSelectedArea(Vertex pt, int botLeft[2], int topRight[2])
{
	if((pt.x < botLeft[0] && pt.x < topRight[0]) || (pt.x > botLeft[0] && pt.x > topRight[0]))
		return false;
	if((pt.y < botLeft[1] && pt.y < topRight[1]) || (pt.y > botLeft[1] && pt.y > topRight[1]))
		return false;
	return true;
}

void findSelectedMinMax(float tempX, float tempY, float tempZ)
{
	if(!selMinMaxFlag)	{		// First initialisation
		cout << "Reinitialising selected min max\n";
		selectedMaxX = tempX;
		selectedMinX = tempX;
		selectedMaxY = tempY;
		selectedMinY = tempY;
		selectedMaxZ = tempZ;
		selectedMinZ = tempZ;
		selMinMaxFlag = true;
	} else {
		if(tempX >= selectedMaxX)	selectedMaxX = tempX;
		if(tempX <  selectedMinX)	selectedMinX = tempX;
		if(tempY >= selectedMaxY)	selectedMaxY = tempY;
		if(tempY <  selectedMinY)	selectedMinY = tempY;
		if(tempZ >= selectedMaxZ)	selectedMaxZ = tempZ;
		if(tempZ <  selectedMinZ)	selectedMinZ = tempZ;
	}
}

bool getSelectedCP(int x, int y)
{
	bool result = false;
	/* Save mouse coordinates as starting point for moving */
	startX = 0;
	startY = 0;
	startX = x;
	startY = y;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);
	
	for(int i = 0; i <= l; i++) {
		for(int j = 0; j <= m; j++) {
			for(int k = 0; k <= n; k++) {
				lattice[i][j][k].selected = false;		// Initialise all CPs as not selected
				lattice2d[i][j][k].selected = false;	// Initialise all CPs as not selected
				
				/* Convert to 2D to check if CP is selected */
				gluProject(lattice[i][j][k].x, lattice[i][j][k].y, lattice[i][j][k].z,
						   modelView, projection, viewport,
						   &lattice2d[i][j][k].x, &lattice2d[i][j][k].y, &lattice2d[i][j][k].z);
				/*cout << "Lattice[" << i << "][" << j << "][" << k << "]:\n"
					 << "3D point: " << lattice[i][j][k].x << ", " << lattice[i][j][k].y << ", " << lattice[i][j][k].z << "\n"
					 << "2D point: " << lattice2d[i][j][k].x << ", " << lattice2d[i][j][k].y << ", " << lattice2d[i][j][k].z << "\n";*/
				lattice2d[i][j][k].selected = checkSelectedCP(lattice2d[i][j][k], startX, startY);
				
				/* Process selected CP */
				if(lattice2d[i][j][k].selected) {
					lattice[i][j][k].selected = true;
					sel_i = i;
					sel_j = j;
					sel_k = k;
					cout << "Get CP: [" << i << "][" << j << "][" << k << "]\n";
					result = true;
					break;
				}
			}
		}
	}

	//drawLattice();
	return result;
}

bool checkSelectedCP(Vertex pt, int x, int y)
{
	if((x < pt.x-5) || (x > pt.x+5))
		return false;
	if((y < pt.y-5) || (y > pt.y+5))
		return false;
	return true;
}

void moveSelectedCP(int endX, int endY)
{
	
	/* Initialise movement variables */
	float moveX = 0;
	float moveY = 0;
	moveX = (endX-startX);
	moveY = (endY-startY);

	
	/* Set end point to start point to find next round of moved distance */
	startX = endX;
	startY = endY;

	if(!(moveX == 0) || !(moveY == 0)) {
		//lattice2d[sel_i][sel_j][sel_k].x = endX;
		//lattice2d[sel_i][sel_j][sel_k].y = endY;
		lattice2d[sel_i][sel_j][sel_k].x += moveX;
		lattice2d[sel_i][sel_j][sel_k].y += moveY;

		/* Convert to 3D to change position of 3D CP */
		gluUnProject(lattice2d[sel_i][sel_j][sel_k].x, lattice2d[sel_i][sel_j][sel_k].y, lattice2d[sel_i][sel_j][sel_k].z,
						modelView, projection, viewport,
						&lattice[sel_i][sel_j][sel_k].x, &lattice[sel_i][sel_j][sel_k].y, &lattice[sel_i][sel_j][sel_k].z);
		cout << "Move CP: [" << sel_i << "][" << sel_j << "][" << sel_k << "]\n";
		//drawLattice();
		//deformMesh();
	}
	/* Position of lattice must move before calling FFD */
	/*if(!(moveX == 0) || !(moveY == 0))
		deformMesh();*/
}

void deformMesh()
{
	copyVertices = new Vertex[numOfVert]();
	for(int count = 0; count < numOfVert; count++) {
		/* Create copy of vertex position */
		copyVertices[count].x = v0[count].x;
		copyVertices[count].y = v0[count].y;
		copyVertices[count].z = v0[count].z;

		/* Recalculate new minimum/maximum vertices */
		if(vertices[count].selected) {
			findSelectedMinMax(vertices[count].x, vertices[count].y, vertices[count].z);
		}
	}
	for(int count = 0; count < numOfVert; count++) {
		/* Only calculate new position of vertex if selected */
		if(vertices[count].selected) {
			/* Highlights selected vertex */
			glColor3f(0.5, 0.5, 1);
			glPushMatrix();
				glTranslatef(copyVertices[count].x, copyVertices[count].y, copyVertices[count].z);
				glutSolidSphere(0.1, 20, 20);
			glPopMatrix();
			glColor3f(1, 1, 1);

			vertices[count].x = 0;
			vertices[count].y = 0;
			//vertices[count].z = 0;

			/* Calculate the FFD position and update vertex position */
			Vertex tempResult = trivariateBernstein(copyVertices[count]);
			vertices[count].x = tempResult.x;
			vertices[count].y = tempResult.y;
			vertices[count].z = tempResult.z;
			//cout << "Vertex (old): " << vertices[count].x << ", " << vertices[count].y << ", " << vertices[count].z << "\n";
			//cout << "Vertex (new): " << vertices[count].x << ", " << vertices[count].y << ", " << vertices[count].z << "\n";
		}
	}

	/* Recompute lattice */
	//initFFD(selectedMaxX, selectedMaxY, selectedMaxZ, selectedMinX, selectedMinY, selectedMinZ);
	//drawLattice();

}