#include "HalfEdge.h"
#include "Mesh.h"
#include "GluiSetup.h"
#include "Ffd.h"
#include <Windows.h>
#include <commdlg.h>

// Dialogs
HWND hWND = NULL;
OPENFILENAME _open_file;
static char  openFile[256];
static char  openFileTitle[256];

// Mesh variables
HE_vert *vertices;
HE_vert *v0;
static HE_face *faces;
static HE_edge *edges;
extern int numOfVert = 0;
extern int numOfFace = 0;
extern int numOfEdge = 0;
extern bool haveMesh = false;
extern bool fileHaveNorm = false;

// Bounding Box Variables
extern bool	 minMaxFlag = false;
extern float maxX = 0.0;
extern float maxY = 0.0;
extern float maxZ = 0.0;
extern float minX = 0.0;
extern float minY = 0.0;
extern float minZ = 0.0;

// Get GLUI IDs
extern int ID_RMODE;
extern bool sCol;

void calcFaceNorm()
{
	for(int count = 0; count < numOfFace; count++)
	{
		/* Get first vector */
		float vx = vertices[faces[count].v2-1].x-vertices[faces[count].v1-1].x;
		float vy = vertices[faces[count].v2-1].y-vertices[faces[count].v1-1].y;
		float vz = vertices[faces[count].v2-1].z-vertices[faces[count].v1-1].z;

		/* Get second vector */
		float ux = vertices[faces[count].v3-1].x-vertices[faces[count].v1-1].x;
		float uy = vertices[faces[count].v3-1].y-vertices[faces[count].v1-1].y;
		float uz = vertices[faces[count].v3-1].z-vertices[faces[count].v1-1].z;
	
		/* Get cross product vector */
		float crossx = vy*uz - vz*uy;
		float crossy = vz*ux - vx*uz;
		float crossz = vx*uy - vy*ux;

		/* Get length of cross product vector */
		float crossl = sqrt(pow(crossx,2) + pow(crossy,2) + pow(crossz,2));
	
		/* Get normalised cross product vector */
		faces[count].nx = crossx / crossl;
		faces[count].ny = crossy / crossl;
		faces[count].nz = crossz / crossl;
	}
}

void calcVertNorm()
{
	for(int count = 0; count < numOfVert; count++)
	{
		HE_edge* outgoing = vertices[count].edge;
		HE_edge* current = outgoing;
		
		int noOfNeighbours = 0;
		float tempnx = 0, tempny = 0, tempnz = 0;
		
		/* Traverse for 1-ring neighbours */
		if(current->pair != NULL) {
			while(current->pair->next != outgoing)
			{
				current = current->pair->next;
				tempnx += current->face->nx;
				tempny += current->face->ny;
				tempnz += current->face->nz;
				noOfNeighbours++;
			}
		} else {
			while(current != NULL) 
			{
				tempnx += current->face->nx;
				tempny += current->face->ny;
				tempnz += current->face->nz;
				current = current->next->next->pair;
				noOfNeighbours++;
			}
		}

		/* Store the vertex normals */
		vertices[count].nx = tempnx/noOfNeighbours;
		vertices[count].ny = tempny/noOfNeighbours;
		vertices[count].nz = tempnz/noOfNeighbours;
	}
}

void loadMesh(std::string path)
{
	FILE *file;
	char type;
	int ID_vertex = 0, ID_face = 0, edgeNum = 0;
	int tempV1 = 0, tempV2 = 0, tempV3 = 0;
	float tempX = 0.0, tempY = 0.0, tempZ = 0.0;
	float tempNX = 0.0, tempNY = 0.0, tempNZ = 0.0;

	map<HE_line,int>edgeMap;
	map<HE_line,int>::iterator edgeMapIter;
	edgeMap.clear();
	int startVert = 0, endVert = 0;

	/* Open file first time */
	file = fopen(path.c_str(), "r");
	if(file == NULL) perror("Error opening file");
	else {
		while((type = fgetc(file)) != EOF) {
			switch(type)
			{
				case 'V':
					if(fscanf(file, "ertex %d %f %f %f\n", &ID_vertex, &tempX, &tempY, &tempZ) == 4) {}
					break;
				case 'F':
					if(fscanf(file, "ace %d %d %d %d\n", &ID_face, &tempV1, &tempV2, &tempV3) == 4) {}
					break;
				case '#':
					do type = fgetc(file); while(type != '\n' && type != EOF);
					break;
			}
		}
	}
	numOfVert = ID_vertex;
	numOfFace = ID_face;
	vertices  = new HE_vert[numOfVert]();
	v0		  = new HE_vert[numOfVert]();
	faces	  = new HE_face[numOfFace]();
	edges	  = new HE_edge[numOfFace*3]();

	/* Open file second time */
	file = fopen(path.c_str(), "r");
	if(file == NULL) perror("Error opening file");
	else {
		while((type = fgetc(file)) != EOF) {
			switch(type)
			{
				case 'V':
					if(fscanf(file,"ertex %d %f %f %f {normal=(%f %f %f)}\n", &ID_vertex, &tempX, &tempY, &tempZ, &tempNX, &tempNY, &tempNZ) == 7) {
						fileHaveNorm = true;
						vertices[ID_vertex-1].x = tempX;
						vertices[ID_vertex-1].y = tempY;
						vertices[ID_vertex-1].z = tempZ;
						vertices[ID_vertex-1].nx = tempNX;
						vertices[ID_vertex-1].ny = tempNY;
						vertices[ID_vertex-1].nz = tempNZ;
						vertices[ID_vertex-1].selected = false;
						vertices[ID_vertex-1].vertID = ID_vertex-1;
						findMinMax(tempX, tempY, tempZ);
					} else {
						vertices[ID_vertex-1].x = tempX;
						vertices[ID_vertex-1].y = tempY;
						vertices[ID_vertex-1].z = tempZ;
						vertices[ID_vertex-1].selected = false;
						vertices[ID_vertex-1].vertID = ID_vertex-1;
						findMinMax(tempX, tempY, tempZ);
					}
					break;
				case 'F':
					if(fscanf(file, "ace %d %d %d %d\n", &ID_face, &tempV1, &tempV2, &tempV3) == 4) {
						faces[ID_face-1].v1 = tempV1;
						faces[ID_face-1].v2 = tempV2;
						faces[ID_face-1].v3 = tempV3;
						faces[ID_face-1].faceID = ID_face-1;

						/* First edge */
						edges[(ID_face-1)*3].vertS = &vertices[faces[ID_face-1].v1-1];
						edges[(ID_face-1)*3].vertE = &vertices[faces[ID_face-1].v2-1];
						startVert = vertices[faces[ID_face-1].v1-1].vertID;
						endVert   = vertices[faces[ID_face-1].v2-1].vertID;
						edges[(ID_face-1)*3].vertS	->edge = &edges[(ID_face-1)*3];
						edges[(ID_face-1)*3].edgeID = (ID_face-1)*3;
						edges[(ID_face-1)*3].face	= &faces[ID_face-1];
						edgeNum++;
						edgeMapIter = edgeMap.find(HE_line(endVert,startVert));
						if(edgeMapIter != edgeMap.end()) {
							edges[(ID_face-1)*3].pair = &edges[edgeMapIter->second-1];
							edges[edgeMapIter->second-1].pair = &edges[(ID_face-1)*3];
							edgeMap.erase(edgeMapIter);
						} else
							edgeMap.insert(pair<HE_line,int>(HE_line(startVert,endVert), edgeNum));

						/* Second edge */
						edges[(ID_face-1)*3+1].vertS = &vertices[faces[ID_face-1].v2-1];
						edges[(ID_face-1)*3+1].vertE = &vertices[faces[ID_face-1].v3-1];
						startVert = vertices[faces[ID_face-1].v2-1].vertID;
						endVert	  = vertices[faces[ID_face-1].v3-1].vertID;
						edges[(ID_face-1)*3+1].vertS  ->edge = &edges[(ID_face-1)*3+1];
						edges[(ID_face-1)*3+1].edgeID = (ID_face-1)*3+1;
						edges[(ID_face-1)*3+1].face	  = &faces[ID_face-1];
						edgeNum++;
						edgeMapIter = edgeMap.find(HE_line(endVert,startVert));
						if(edgeMapIter != edgeMap.end()) {
							edges[(ID_face-1)*3+1].pair = &edges[edgeMapIter->second-1];
							edges[edgeMapIter->second-1].pair = &edges[(ID_face-1)*3+1];
							edgeMap.erase(edgeMapIter);
						} else
							edgeMap.insert(pair<HE_line,int>(HE_line(startVert,endVert), edgeNum));

						/* Third edge */
						edges[(ID_face-1)*3+2].vertS = &vertices[faces[ID_face-1].v3-1];
						edges[(ID_face-1)*3+2].vertE = &vertices[faces[ID_face-1].v1-1];
						startVert = vertices[faces[ID_face-1].v3-1].vertID;
						endVert	  = vertices[faces[ID_face-1].v1-1].vertID;
						edges[(ID_face-1)*3+2].vertS  ->edge = &edges[(ID_face-1)*3+2];
						edges[(ID_face-1)*3+2].edgeID = (ID_face-1)*3+2;
						edges[(ID_face-1)*3+2].face	  = &faces[ID_face-1];
						edgeNum++;
						edgeMapIter = edgeMap.find(HE_line(endVert,startVert));
						if(edgeMapIter != edgeMap.end()) {
							edges[(ID_face-1)*3+2].pair = &edges[edgeMapIter->second-1];
							edges[edgeMapIter->second-1].pair = &edges[(ID_face-1)*3+2];
							edgeMap.erase(edgeMapIter);
						} else
							edgeMap.insert(pair<HE_line,int>(HE_line(startVert,endVert), edgeNum));

						/* Set next edge */
						edges[(ID_face-1)*3].next	= &edges[(ID_face-1)*3+1];
						edges[(ID_face-1)*3+1].next	= &edges[(ID_face-1)*3+2];
						edges[(ID_face-1)*3+2].next	= &edges[(ID_face-1)*3];

						/* Set face edge */
						faces[ID_face-1].edge = &edges[(ID_face-1)*3];
					}
					break;
				case '#':
					do type = fgetc(file); while(type != '\n' && type != EOF);
					break;
			}
		}
	}
	numOfVert = ID_vertex;
	numOfFace = ID_face;
	numOfEdge = edgeNum;
	edgeMap.clear();

	/* For boundary edge */
	for(int num = 0; num < numOfEdge; num++) {
		if(edges[num].pair == NULL)
			vertices[edges[num].vertS->vertID].edge = &edges[num];
	}

	for(int ii =0; ii<numOfVert; ii++){
		v0[ii].x = vertices[ii].x;
		v0[ii].y = vertices[ii].y;
		v0[ii].z = vertices[ii].z;
	}


	printf("No. of vertices = %d\n", numOfVert);
	printf("No. of faces = %d\n", numOfFace);
	printf("No. of edges = %d\n", numOfEdge);
}

void findMinMax(float tempX, float tempY, float tempZ)
{
	if(!minMaxFlag)	{	// Initialisation
		maxX = tempX;
		minX = tempX;
		maxY = tempY;
		minY = tempY;
		maxZ = tempZ;
		minZ = tempZ;
		minMaxFlag = true;
	} else {
		if(tempX >= maxX)	maxX = tempX;
		if(tempX <  minX)	minX = tempX;
		if(tempY >= maxY)	maxY = tempY;
		if(tempY <  minY)	minY = tempY;
		if(tempZ >= maxZ)	maxZ = tempZ;
		if(tempZ <  minZ)	minZ = tempZ;
	}
}

void setToOrigin()
{
	float midX = (maxX-minX)/2;
	float midY = (maxY-minY)/2;
	float midZ = (maxZ-minZ)/2;

	/* Set mesh to origin */
	for(int count = 0; count < numOfVert; count++) {
		vertices[count].x -= (minX+midX);
		vertices[count].y -= (minY+midY);
		vertices[count].z -= (minZ+midZ);
	}

	/* Set bounding box to origin */
	maxX -= (minX+midX); maxY -= (minY+midY); maxZ -= (minZ+midZ);
	minX -= (minX+midX); minY -= (minY+midY); minZ -= (minZ+midZ);
}

void scaleToOrigin()
{
	if(maxX-minX >=20)
	{
		do {
			for(int count = 0; count < numOfVert; count++) {
				vertices[count].x /= 2;
				vertices[count].y /= 2;
				vertices[count].z /= 2;
			}
			maxX /= 2; maxY /= 2; maxZ /= 2;
			minX /= 2; minY /= 2; minZ /= 2;
		} while(maxX-minX >= 20);
	}
	if(maxX-minX <= 10)
	{
		do {
			for(int count = 0; count < numOfVert; count++) {
				vertices[count].x *= 2;
				vertices[count].y *= 2;
				vertices[count].z *= 2;
			}
			maxX *= 2; maxY *= 2; maxZ *= 2;
			minX *= 2; minY *= 2; minZ *= 2;
		} while(maxX-minX <= 10);
	}
}

double editLineWidth(int width)
{
	double lineWidth;
	glGetDoublev(GL_LINE_WIDTH, &lineWidth);	// Store the existing line width
	glLineWidth(width);							// Set current line width to new width
	return lineWidth;							// Return the existing line width
}

void drawMeshWire()
{
	if(sCol && !(ID_RMODE == 2)) {
		for(int num = 0; num < numOfFace; num++) {
			HE_edge* tempEdge = faces[num].edge;
			glBegin(GL_LINE_LOOP);
				glColor3f((tempEdge->vertE->x-minX)/maxX, (tempEdge->vertE->y-minY)/maxY, (tempEdge->vertE->z-minZ)/maxZ);
				glNormal3f(tempEdge->vertE->nx, tempEdge->vertE->ny, tempEdge->vertE->nz);
				glVertex3f(tempEdge->vertE->x,  tempEdge->vertE->y,  tempEdge->vertE->z);
				glColor3f((tempEdge->next->vertE->x-minX)/maxX, (tempEdge->next->vertE->y-minY)/maxY, (tempEdge->next->vertE->z-minZ)/maxZ);
				glNormal3f(tempEdge->next->vertE->nx, tempEdge->next->vertE->ny, tempEdge->next->vertE->nz);
				glVertex3f(tempEdge->next->vertE->x,  tempEdge->next->vertE->y,  tempEdge->next->vertE->z);
				glColor3f((tempEdge->next->next->vertE->x-minX)/maxX, (tempEdge->next->next->vertE->y-minY)/maxY, (tempEdge->next->next->vertE->z-minZ)/maxZ);
				glNormal3f(tempEdge->next->next->vertE->nx, tempEdge->next->next->vertE->ny, tempEdge->next->next->vertE->nz);
				glVertex3f(tempEdge->next->next->vertE->x,  tempEdge->next->next->vertE->y,  tempEdge->next->next->vertE->z);
			glEnd();
		}
	} else {
		for(int num = 0; num < numOfFace; num++) {
			HE_edge* tempEdge = faces[num].edge;
			glBegin(GL_LINE_LOOP);
				glNormal3f(tempEdge->vertE->nx, tempEdge->vertE->ny, tempEdge->vertE->nz);
				glVertex3f(tempEdge->vertE->x,  tempEdge->vertE->y,  tempEdge->vertE->z);
				glNormal3f(tempEdge->next->vertE->nx, tempEdge->next->vertE->ny, tempEdge->next->vertE->nz);
				glVertex3f(tempEdge->next->vertE->x,  tempEdge->next->vertE->y,  tempEdge->next->vertE->z);
				glNormal3f(tempEdge->next->next->vertE->nx, tempEdge->next->next->vertE->ny, tempEdge->next->next->vertE->nz);
				glVertex3f(tempEdge->next->next->vertE->x,  tempEdge->next->next->vertE->y,  tempEdge->next->next->vertE->z);
			glEnd();
		}
	}
}

void drawMeshSolid()
{
	if(sCol) {
		for(int num = 0; num < numOfFace; num++) {
			HE_edge* tempEdge = faces[num].edge;
			glBegin(GL_TRIANGLES);
				glColor3f((tempEdge->vertE->x-minX)/maxX, (tempEdge->vertE->y-minY)/maxY, (tempEdge->vertE->z-minZ)/maxZ);
				glNormal3f(tempEdge->vertE->nx, tempEdge->vertE->ny, tempEdge->vertE->nz);
				glVertex3f(tempEdge->vertE->x,  tempEdge->vertE->y,  tempEdge->vertE->z);
				glColor3f((tempEdge->next->vertE->x-minX)/maxX, (tempEdge->next->vertE->y-minY)/maxY, (tempEdge->next->vertE->z-minZ)/maxZ);
				glNormal3f(tempEdge->next->vertE->nx, tempEdge->next->vertE->ny, tempEdge->next->vertE->nz);
				glVertex3f(tempEdge->next->vertE->x,  tempEdge->next->vertE->y,  tempEdge->next->vertE->z);
				glColor3f((tempEdge->next->next->vertE->x-minX)/maxX, (tempEdge->next->next->vertE->y-minY)/maxY, (tempEdge->next->next->vertE->z-minZ)/maxZ);
				glNormal3f(tempEdge->next->next->vertE->nx, tempEdge->next->next->vertE->ny, tempEdge->next->next->vertE->nz);
				glVertex3f(tempEdge->next->next->vertE->x,  tempEdge->next->next->vertE->y,  tempEdge->next->next->vertE->z);
			glEnd();
		}
	} else {
		for(int num = 0; num < numOfFace; num++) {
			HE_edge* tempEdge = faces[num].edge;
			glBegin(GL_TRIANGLES);
				glNormal3f(tempEdge->vertE->nx, tempEdge->vertE->ny, tempEdge->vertE->nz);
				glVertex3f(tempEdge->vertE->x,  tempEdge->vertE->y,  tempEdge->vertE->z);
				glNormal3f(tempEdge->next->vertE->nx, tempEdge->next->vertE->ny, tempEdge->next->vertE->nz);
				glVertex3f(tempEdge->next->vertE->x,  tempEdge->next->vertE->y,  tempEdge->next->vertE->z);
				glNormal3f(tempEdge->next->next->vertE->nx, tempEdge->next->next->vertE->ny, tempEdge->next->next->vertE->nz);
				glVertex3f(tempEdge->next->next->vertE->x,  tempEdge->next->next->vertE->y,  tempEdge->next->next->vertE->z);
			glEnd();
		}
	}
}

void drawMeshPoint()
{
	GLfloat radius;	// Store the existing point size
	glGetFloatv(GL_POINT_SIZE, &radius);
	glPointSize(2);	// Set current point size to 2

	if(sCol) {
		for(int num = 0; num < numOfVert; num++) {
			glBegin(GL_POINTS);
				glColor3f((vertices[num].x-minX)/maxX, (vertices[num].y-minY)/maxY, (vertices[num].z-minZ)/maxZ);
				glNormal3f(vertices[num].nx, vertices[num].ny, vertices[num].nz);
				glVertex3f(vertices[num].x,  vertices[num].y,  vertices[num].z);
				glColor3f(1, 1, 1);
			glEnd();
		}
	} else {
		for(int num = 0; num < numOfVert; num++) {
			glBegin(GL_POINTS);
				glNormal3f(vertices[num].nx, vertices[num].ny, vertices[num].nz);
				glVertex3f(vertices[num].x,  vertices[num].y,  vertices[num].z);
			glEnd();
		}
	}
}

void drawBbox()
{
	glColor3f(0.4, 0.4, 1);
	double lineWidth = editLineWidth(2);
	glPushMatrix();

	/* Draw top of the box */
	glBegin(GL_LINE_LOOP);
		glVertex3f(minX, maxY, maxZ);
		glVertex3f(minX, maxY, minZ);
		glVertex3f(maxX, maxY, minZ);
		glVertex3f(maxX, maxY, maxZ);
	glEnd();

	/* Draw bottom of the box */
	glBegin(GL_LINE_LOOP);
		glVertex3f(minX, minY, maxZ);
		glVertex3f(minX, minY, minZ);
		glVertex3f(maxX, minY, minZ);
		glVertex3f(maxX, minY, maxZ);
	glEnd();

	/* Draw lines connecting the top and bottom */
	glBegin(GL_LINES);
		glVertex3f(minX, maxY, maxZ);
		glVertex3f(minX, minY, maxZ);
		glVertex3f(minX, maxY, minZ);
		glVertex3f(minX, minY, minZ);
		glVertex3f(maxX, maxY, minZ);
		glVertex3f(maxX, minY, minZ);
		glVertex3f(maxX, maxY, maxZ);
		glVertex3f(maxX, minY, maxZ);
	glEnd();

	glPopMatrix();

	/* Restore line width */
	glLineWidth(lineWidth);
	glColor3f(1, 1, 1);
}

void drawGrid()
{
	glEnable(GL_LINE_SMOOTH);
	glColor3f(0.9, 0.9, 0.9);
	glPushMatrix();

	/* Draw grid */
	for(GLfloat i = -10; i <= 10; i++)
	{
		glBegin(GL_LINES);
			glVertex3f(i, 0, 10);
			glVertex3f(i, 0, -10);
			glVertex3f(10, 0, i);
			glVertex3f(-10, 0, i);
		glEnd();
	}
	double lineWidth = editLineWidth(3);

	/* Draw main XY axes */
	glColor3f(1, 1, 1);
	glBegin(GL_LINES);
		glVertex3f(0, 0, 10);
		glVertex3f(0, 0, -10);
		glVertex3f(-10, 0, 0);
		glVertex3f(10, 0, 0);
	glEnd();

	glPopMatrix();

	/* Restore line width */
	glLineWidth(lineWidth);
	glDisable(GL_LINE_SMOOTH);
}

void drawAxes()
{
	glEnable(GL_LINE_SMOOTH);
	GLUquadricObj *quadratic = gluNewQuadric();

	/* Draw z-axis */
	glColor3f(1, 0, 0);
	glPushMatrix();
	gluCylinder(quadratic, 0.05f, 0.05f, 3, 30, 30);
	glPushMatrix();
		glTranslatef(0, 0, 3);
		glutSolidCone(0.1f, 0.3f, 30, 30);
	glPopMatrix();

	/* Draw x-axis */
	glColor3f(0, 0, 1);
	glPushMatrix();
		glRotatef(90, 0, 1, 0);
		gluCylinder(quadratic, 0.05f, 0.05f, 3, 30, 30);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(3, 0, 0);
		glRotatef(90, 0, 1, 0);
		glutSolidCone(0.1f, 0.3f, 30, 30);
	glPopMatrix();
	
	/* Draw y-axis */
	glColor3f(0, 1, 0);
	glPushMatrix();
		glRotatef(-90, 1, 0, 0);
		gluCylinder(quadratic, 0.05f, 0.05f, 3, 30, 30);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0, 3, 0);
		glRotatef(-90, 1, 0, 0);
		glutSolidCone(0.1f, 0.3f, 30, 30);
	glPopMatrix();
	glPopMatrix();

	glColor3f(1, 1, 1);
	glDisable(GL_LINE_SMOOTH);
}

void createFileDialog(void)
{
	clearMesh();

	/* Open file dialog */
	memset(&_open_file, 0, sizeof(_open_file));
	_open_file.lStructSize = sizeof(_open_file);
	_open_file.hwndOwner = hWND;
	_open_file.lpstrFilter ="3D Model object(*m)\0*.m;\0\0";
	_open_file.nFilterIndex =1;
	_open_file.lpstrFile = openFile;
	_open_file.nMaxFile = sizeof(openFile);
	_open_file.lpstrFileTitle = openFileTitle;
	_open_file.nMaxFileTitle = sizeof(openFileTitle);
	_open_file.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;

	if(GetOpenFileName(&_open_file)) {
		haveMesh = true;
		loadMesh(string(openFileTitle));
		if(!fileHaveNorm) {
			calcFaceNorm();
			calcVertNorm();
		}
		setToOrigin();
		scaleToOrigin();
		initFFD(maxX, maxY, maxZ, minX, minY, minZ);
	} else haveMesh = false;
}

int createMsgbox(int msgboxID)
{
	int msgbox;
	switch(msgboxID)
	{
		case ERR_no_mesh:
			msgbox = MessageBoxW(NULL,
						L"No mesh is found.\nPlease open a mesh.",
						L"Error",
						MB_ICONEXCLAMATION | MB_OKCANCEL);
			break;
		case ERR_no_pt:
			msgbox = MessageBoxW(NULL,
						L"No lattice point is selected.\nPlease select lattice point.",
						L"Error",
						MB_ICONEXCLAMATION | MB_OKCANCEL);
			break;
	}
	return msgbox;
}