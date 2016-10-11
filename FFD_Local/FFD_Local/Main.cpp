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
#include "Main.h"
#include "Mesh.h"
#include "GluiSetup.h"
#include "Ffd.h"
#include "HalfEdge.h"

// Main variables
GLubyte stipple[128];	// Create a 32x32 (1024) array of bits (128bytes=1024bits)

// Get camera variables
extern float translateX, translateY;
extern float angleX, angleY;
extern float scaleSize;
extern HE_vert *v0, *vertices;

// Get object transformation variables
extern float translate[];
extern float rotationMatrix[16];
extern float ID_SCALE;

// Get mesh variables
extern bool haveMesh;
extern bool fileHaveNorm;
extern int numOfVert;

// Get bounding box variables
extern float maxX, maxY, maxZ;
extern float minX, minY, minZ;

// Get render variables
extern float color[];

// Get GLUI IDs
extern int ID_AXES, ID_BBOX;
extern int ID_COLOR, ID_FOG, ID_TRANSP;
extern int ID_LIGHT0, ID_LIGHT1;
extern int ID_PROJ, ID_SHADE, ID_RMODE;
extern int ID_SELECT_AREA, ID_SELECT_LATT;

// Get GLUI variables
extern GLfloat rLight0, gLight0, bLight0;
extern GLfloat rLight1, gLight1, bLight1;
extern GLUI_Spinner *rLight0Spinner, *gLight0Spinner, *bLight0Spinner;
extern GLUI_Spinner *rLight1Spinner, *gLight1Spinner, *bLight1Spinner;
extern GLUI_RadioGroup *shadingGroup, *projGroup;
extern GLUI_RadioGroup *sMeshGroup, *rModeGroup;
extern GLUI_Checkbox *light0Chkbox, *light1Chkbox;
extern GLUI_Checkbox *axesChkbox, *bboxChkbox, *fogChkbox, *transpChkbox;
extern GLUI_Checkbox *selectAreaChkbox, *selectLattChkbox;

void display()
{
	glutWidth  = glutGet(GLUT_WINDOW_WIDTH);
	glutHeight = glutGet(GLUT_WINDOW_HEIGHT);
	glViewport(0, 0, (GLsizei) glutWidth, (GLsizei) glutHeight);
	
	/* Clean screen (called everytime when drawing) */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	/* Enable GL capabilities */
	glPushAttrib(GL_POLYGON_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);		// Enable depth test
	glEnable(GL_POINT_SMOOTH);		// Enable round vertices
	glEnable(GL_NORMALIZE);			// Enable normalisation
	glEnable(GL_BLEND);				// Enable alpha channel textures
	glEnable(GL_LIGHTING);			// Enable lighting
	glEnable(GL_COLOR_MATERIAL);	// Enable colours on materials

	/* Set mesh color */
	glColor3fv(color);

	/* Set projection */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(ID_PROJ == 0) {
		if(maxY == 0)
			glOrtho(-2.5f*xyAspect, 2.5f*xyAspect, -2.5f, 2.5f, 0.1f, 100.0f);
		else
			glOrtho(-5.0*xyAspect, 5.0*xyAspect, -5.0f, 5.0f, 0.1f, 100.0f);
	} else {
		if(maxY == 0)
			gluPerspective(20.0f, xyAspect, 0.1f, 100.0f);
		else
			gluPerspective(40.0f, xyAspect, 0.1f, 100.0f);
	}

	/* Set modelview matrix */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0,0,15,0,0,0,0,1,0);

	/* Rotate, scale and translate */
	glRotatef(angleX, 0, 1, 0);
	glRotatef(angleY, 1, 0, 0);
	glScalef(scaleSize, scaleSize, scaleSize);
	glTranslatef(translateX, translateY, 0.0);

	/* Set fog */
	if(ID_FOG == 1) {
		glEnable(GL_FOG);
		GLfloat atmoColor[] = {0.8f, 0.8f, 0.9f, 1.0f};
		glFogfv(GL_FOG_COLOR, atmoColor);
		glFogi(GL_FOG_MODE, GL_EXP);
		glFogf(GL_FOG_DENSITY, 0.05);
	} else glDisable(GL_FOG);

	/* Set transparent */
	if(ID_TRANSP == 1) {
		glEnable(GL_POLYGON_STIPPLE);
		for(int i = 0; i < 128;) {	// Populate array with stipple pattern
			stipple[i++] = 85;  stipple[i++] = 85;  stipple[i++] = 85;  stipple[i++] = 85;	// Even row, 85 = 01010101
			stipple[i++] = 170; stipple[i++] = 170; stipple[i++] = 170; stipple[i++] = 170;	// Odd row, 170 = 10101010
		}
		glPolygonStipple(stipple);
	} else glDisable(GL_POLYGON_STIPPLE);

	/* Set shading */
	if(ID_SHADE == 0)
		glShadeModel(GL_SMOOTH);
	else glShadeModel(GL_FLAT);

	/* Set Light0 */
	if(ID_LIGHT0 == 1) {
		glEnable(GL_LIGHT0);
		rLight0Spinner->set_float_val(rLight0);
		gLight0Spinner->set_float_val(gLight0);
		bLight0Spinner->set_float_val(bLight0);
	} else glDisable(GL_LIGHT0);
	GLfloat light0_diffuse[] = {rLight0, gLight0, bLight0};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);

	/* Set Light1 */
	if(ID_LIGHT1 == 1) {
		glEnable(GL_LIGHT1);
		rLight1Spinner->set_float_val(rLight1);
		gLight1Spinner->set_float_val(gLight1);
		bLight1Spinner->set_float_val(bLight1);
	} else glDisable(GL_LIGHT1);
	GLfloat light1_diffuse[] = {rLight1, gLight1, bLight1};
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);

	/* Draw mesh */
	if(haveMesh)
	{
		glPushMatrix();
		glMultMatrixf(rotationMatrix);
		glScalef(ID_SCALE, ID_SCALE, ID_SCALE);
		glTranslatef(translate[0], translate[1], -translate[2]);
		if(ID_RMODE == 0)			// Wireframe mode
			drawMeshWire();
		else if(ID_RMODE == 1)		// Solid mode
			drawMeshSolid();
		else if(ID_RMODE == 2) {	// Edge mode
			drawMeshSolid();
			glColor3f(1, 1, 1);
			if(ID_COLOR == 12) glColor3f(0, 0, 0);
			double lineWidth = editLineWidth(2);
			drawMeshWire();
			glLineWidth(lineWidth);
			glColor3f(1, 1, 1);
		} else drawMeshPoint();		// Point mode

		/* Set bounding box */
		if(ID_BBOX == 1) {
			glDisable(GL_LIGHTING);
			glDisable(GL_POLYGON_STIPPLE);
			glDisable(GL_FOG);
			drawBbox();
			glEnable(GL_FOG);
			glEnable(GL_POLYGON_STIPPLE);
			glEnable(GL_LIGHTING);
		} else glutPostRedisplay();
		glPopMatrix();
	}
	glDisable(GL_POLYGON_STIPPLE);
	glDisable(GL_FOG);
	glPopAttrib();
	
	/* Set axes */
	if(ID_AXES == 1) {
		glDisable(GL_LIGHTING);
		drawGrid();
		drawAxes();
		glEnable(GL_LIGHTING);
	} else glutPostRedisplay();
	
	/* Selecting points */
	glPushMatrix();
	glMultMatrixf(rotationMatrix);
	glScalef(ID_SCALE, ID_SCALE, ID_SCALE);
	glTranslatef(translate[0], translate[1], -translate[2]);
	if(selecting && ID_SELECT_AREA == 1) {
		//cout << "Select Area Checkbox checked & Selecting flag is true\n";
		drawSelectedArea(botLeft, topRight);			// Draw the selecting rectangle
		glColor3f(1, 1, 1);
	} else if(!selecting && ID_SELECT_AREA == 1) {		// After mouse up of selecting area
		//cout << "Select Area Checkbox checked & Selecting flag is false\n";
		foundArea = false;
		foundArea = getSelectedArea(botLeft, topRight);	// Get the selected mesh vertices
		if(foundArea) {									// If vertices are selected, enable select CP
			botLeft[0]  = botLeft[1]  = 0;
			topRight[0] = topRight[1] = 0;
			selectLatt  = true;					// Enable select CP flag				
			selectAreaChkbox->set_int_val(0);	// Disable selecting area checkbox
			selectLattChkbox->set_int_val(1);	// Enable selecting CP checkbox
		} else {								// No vertices are selected, don't allow select CP
			if(createMsgbox(1) == IDOK) {
				botLeft[0]  = botLeft[1]  = 0;
				topRight[0] = topRight[1] = 0;
				selecting = true;				// Go back to selecting area
			}
		}
	} else if(selectLatt && ID_SELECT_LATT == 1) {				// After getting selected area
		cout << "Select Lattice Checkbox checked & Select Lattice flag is true\n";
		//foundCP = false;
		//foundCP = getSelectedCP(selBotLeft[0], selBotLeft[1]);	// Detect which CP is selected
		if(foundCP) {
			selectLatt = false;
			movingLatt = true;
		}
	} //else if(movingLatt && ID_SELECT_LATT == 1) {				// After getting selected CP
		//cout << "Select Lattice Checkbox checked & Move Lattice flag is true\n";
		//moveSelectedCP(selTopRight[0], selTopRight[1]);			// Move selected CP
		//deformMesh();
		//glutPostRedisplay();
	//}
	else {
		//cout << "Not performing any FFD functions\n";
		selecting = true;
		//selectLatt = true;
		//movingLatt = false;
		selBotLeft[0]  = selBotLeft[1]  = 0;
		selTopRight[0] = selTopRight[1] = 0;
		glutPostRedisplay();
	}
	glPopMatrix();

	/* Set lattice */
	if(ID_SELECT_LATT == 1) //!selectLatt && !movingLatt && 
		drawLattice();
	else glutPostRedisplay();

	glutSwapBuffers();
}

void reshape(int width, int height)
{
	GLint viewportX, viewportY, viewportW, viewportH;
	/* Get the viewport area */
	GLUI_Master.get_viewport_area(&viewportX, &viewportY, &viewportW, &viewportH);
	/* Set viewport in proportion */
	glViewport(viewportX, viewportY, viewportW, viewportH);
	/* Get window proportion */
	xyAspect = (float)width/(float)height;
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
	/* Normal scene transformation */
	if(ID_SELECT_AREA == 0 && ID_SELECT_LATT == 0) {
		if(state == GLUT_DOWN) {
			mouseX = x;	mouseY = y;
			if(button == GLUT_LEFT_BUTTON)			// Left click for rotation
				transformMode = TRANSFORM_ROTATE;
			else if(button == GLUT_MIDDLE_BUTTON)	// Middle click for translate
				transformMode = TRANSFORM_TRANSLATE;
			else if(button == GLUT_RIGHT_BUTTON)	// Right click for scale/zoom
				transformMode = TRANSFORM_SCALE;
		} else if(state == GLUT_UP)
			transformMode = TRANSFORM_NONE;
	}

	/* Selecting area to perform FFD */
	else if(ID_SELECT_AREA == 1 && ID_SELECT_LATT == 0) {
		if(button == GLUT_LEFT_BUTTON) {			// Left click to select
			cout << "MOUSE: click - select area\n";
			if(state == GLUT_DOWN) {
				botLeft[0]  = botLeft[1]  = 0;
				topRight[0] = topRight[1] = 0;

				for(int count = 0; count < numOfVert; count++)
				{
					v0[count] = vertices[count];
				}

				transformMode = TRANSFORM_SELECT;
				botLeft[0] = topRight[0] = x;
				botLeft[1] = topRight[1] = glutHeight-y;
				selecting = true;					// Set selecting area flag
			} else if(state == GLUT_UP) {
				if(selecting) {
					topRight[0] = x;
					topRight[1] = glutHeight-y;
					selecting = false;				// Done selecting area
					transformMode = TRANSFORM_NONE;
				}
			}
		} else if(button == GLUT_MIDDLE_BUTTON) {
			cout << "MOUSE: click - translating (in select area mode)\n";
			mouseX = x;	mouseY = y;
			transformMode = TRANSFORM_TRANSLATE;
		} else if(button == GLUT_RIGHT_BUTTON) {
			cout << "MOUSE: click - rotating (in select area mode)\n";
			mouseX = x;	mouseY = y;
			transformMode = TRANSFORM_ROTATE;
		} else transformMode = TRANSFORM_NONE;		// All other clicks do nothing
	}
	
	/* Selecting lattice points */
	else if(ID_SELECT_LATT == 1 && ID_SELECT_AREA == 0) {
		if(button == GLUT_LEFT_BUTTON) {			// Left click to select
			cout << "MOUSE: click - select lattice point\n";
			if(selectLatt == 1 && state == GLUT_DOWN) {
				//foundCP = false;
				foundCP = getSelectedCP(x, glutHeight-y);	// Detect which CP is selected
				//selBotLeft[0]  = selBotLeft[1]  = 0;
				//selTopRight[0] = selTopRight[1] = 0;
				transformMode = TRANSFORM_SELECT_LATT;
				//selBotLeft[0] = selTopRight[0] = x;
				//selBotLeft[1] = selTopRight[1] = glutHeight-y;
				//selectLatt = true;					// Set selecting lattice points flag
				//movingLatt = false;					// Make sure not yet moving lattice points
			} else if(state == GLUT_UP) {
				if(movingLatt) {
					//selTopRight[0] = x;
					//selTopRight[1] = glutHeight-y;
					foundCP = false;
					movingLatt = false;				// Done moving selected CP
					selectLatt = true;				// Back to selecting lattice points
					transformMode = TRANSFORM_NONE;
				}
			}
		} else if(button == GLUT_MIDDLE_BUTTON) {
			cout << "MOUSE: click - translating (in select lattice mode)\n";
			mouseX = x;	mouseY = y;
			transformMode = TRANSFORM_TRANSLATE;
		} else if(button == GLUT_RIGHT_BUTTON) {
			cout << "MOUSE: click - rotating (in select lattice mode)\n";
			mouseX = x;	mouseY = y;
			transformMode = TRANSFORM_ROTATE;
		} else transformMode = TRANSFORM_NONE;		// All other clicks do nothing
	}
	glutPostRedisplay();
}

void motion(int x, int y)
{
	if(transformMode == TRANSFORM_ROTATE) {
		/* X-direction */
		angleX += (x-mouseX)/5.0;
		if(angleX > 180)
			angleX -= 360;
		else if(angleX < -180)
			angleX += 360;

		/* Y-direction */
		angleY += (y-mouseY)/5.0;
		if(angleY > 180)
			angleY -= 360;
		else if(angleY < -180)
			angleY += 360;
		
		mouseX = x;
		mouseY = y;
	} else if(transformMode == TRANSFORM_TRANSLATE) {
		translateX = (x-mouseX)/30.0;
		translateY = (mouseY-y)/30.0;
	} else if(transformMode == TRANSFORM_SCALE) {
		float oldSize = scaleSize;
		scaleSize *= (1 + (y-mouseY)/60.0);
		if(scaleSize < 0)
			scaleSize = oldSize;
		mouseY = y;
	} else if(transformMode == TRANSFORM_SELECT) {
		cout << "MOUSE: motion - select area\n";
		if(selecting) {
			topRight[0] = x;
			topRight[1] = glutHeight-y;
		}
	} else if(transformMode == TRANSFORM_SELECT_LATT) {
		cout << "MOUSE: motion - select lattice point\n";
		if(movingLatt) {
			moveSelectedCP(x, glutHeight-y);			// Move selected CP
			//glMatrixMode(GL_MODELVIEW);
			//glFlush();
			deformMesh();
			//selTopRight[0] = x;
			//selTopRight[1] = glutHeight-y;
		}
	}
	glutPostRedisplay();
}

void hotkey(unsigned char key, int x, int y)
{
	switch(key)
	{
		case '[': printf("KEY: [ - Shading smooth\n");
			shadingGroup->set_int_val(0);
			break;
		case ']': printf("KEY: ] - Shading flat\n");
			shadingGroup->set_int_val(1);
			break;
		case '-': printf("KEY: O - Projecting orthogonal\n");
			projGroup->set_int_val(0);
			break;
		case '=': printf("KEY: P - Projecting perspective\n");
			projGroup->set_int_val(1);
			break;
		case '0':
			if(ID_LIGHT0 == 0) {
				printf("KEY: 0 - Enable Light0\n");
				light0Chkbox->set_int_val(1);
			} else {
				printf("KEY: 0 - Disable Light0\n");
				light0Chkbox->set_int_val(0);
			} break;
		case '1':
			if(ID_LIGHT1 == 0) {
				printf("KEY: 1 - Enable Light1\n");
				light1Chkbox->set_int_val(1);
			} else {
				printf("KEY: 1 - Disable Light1\n");
				light1Chkbox->set_int_val(0);
			} break;
		case 'A':
		case 'a':
			if(ID_AXES == 0) {
				printf("KEY: A - Enable axes\n");
				axesChkbox->set_int_val(1);
			} else {
				printf("KEY: A - Disable axes\n");
				axesChkbox->set_int_val(0);
			} break;
		case 'B':
		case 'b':
			if(haveMesh) {
				if(ID_BBOX == 0) {
					printf("KEY: B - Enable bounding box\n");
					bboxChkbox->set_int_val(1);
				} else {
					printf("KEY: B - Disable bounding box\n");
					bboxChkbox->set_int_val(0);
				}
			} else {
				if(createMsgbox(ERR_no_mesh) == IDOK) {
					createFileDialog();
					if(haveMesh)
						bboxChkbox->set_int_val(1);
					else
						bboxChkbox->set_int_val(0);
				} else {
					bboxChkbox->set_int_val(0);
				}
			} break;
		case 'C':
		case 'c': printf("KEY: C - Clearing mesh\n");
			sMeshGroup->set_int_val(-1);
			reset();
			break;
		case 'E':
		case 'e': printf("KEY: E - Rendering edge\n");
			rModeGroup->set_int_val(2);
			break;
		case 'F':
		case 'f':
			if(ID_FOG == 0) {
				printf("KEY: F - Enable fog\n");
				fogChkbox->set_int_val(1);
			} else {
				printf("KEY: F - Disable fog\n");
				fogChkbox->set_int_val(0);
			} break;
		case 'K':
		case 'k':
			if(haveMesh) {
				if(ID_SELECT_AREA == 0) {
					printf("KEY: K - Enable select area\n");
					selectAreaChkbox->set_int_val(1);
				} else {
					printf("KEY: K - Disable select area\n");
					selectAreaChkbox->set_int_val(0);
				}
			} else {
				if(createMsgbox(ERR_no_mesh) == IDOK) {
					createFileDialog();
					if(haveMesh)
						selectAreaChkbox->set_int_val(1);
					else
						selectAreaChkbox->set_int_val(0);
				} else {
					selectAreaChkbox->set_int_val(0);
				}
			} break;
		case 'L':
		case'l':
			if(haveMesh) {
				if(ID_SELECT_LATT == 0) {
					printf("KEY: L - Enable select lattice\n");
					selectLattChkbox->set_int_val(1);
				} else {
					printf("KEY: L - Disable select lattice\n");
					selectLattChkbox->set_int_val(0);
				}
			} else {
				if(createMsgbox(ERR_no_mesh) == IDOK) {
					createFileDialog();
					if(haveMesh)
						selectLattChkbox->set_int_val(1);
					else
						selectLattChkbox->set_int_val(0);
				} else {
					selectLattChkbox->set_int_val(0);
				}
			}
			break;
		case 'O':
		case 'o': printf("KEY: O - Opening mesh\n");
			createFileDialog();
			break;
		case 'P':
		case 'p': printf("KEY: D - Rendering point\n");
			rModeGroup->set_int_val(3);
			break;
		case 'Q':
		case 'q': printf("KEY: Q - Quit\n");
			exit(1);
			break;
		case 'S':
		case 's': printf("KEY: S - Rendering solid\n");
			rModeGroup->set_int_val(1);
			break;
		case 'T':
		case 't':
			if(ID_TRANSP == 0) {
				printf("KEY: T - Enable transparent\n");
				transpChkbox->set_int_val(1);
			} else {
				printf("KEY: T - Disable transparent\n");
				transpChkbox->set_int_val(0);
			} break;
		case 'W':
		case 'w': printf("KEY: W - Rendering wireframe\n");
			rModeGroup->set_int_val(0);
			break;
	}
	GLUI_Master.sync_live_all();	// Syncs the button UI
	glutPostRedisplay();			// Redraw
}

void getResolution(int &horizontal, int &vertical)
{
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();	// Get a handle to desktop window
	GetWindowRect(hDesktop, &desktop);			// Get the size of desktop screen
	horizontal	= desktop.right;				// Note: top left corner == (0,0)
	vertical	= desktop.bottom;
}

int main(int argc, char **argv)
{
	/* Get desktop resolution to initialise window size */
	getResolution(winHor, winVer);

	/* Initialise GLUT and create window */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);	// Set the display mode
	glutInitWindowSize(winHor-150, winVer-100);					// Specify window size (minus 100 from screensize)
	glutInitWindowPosition(0,0);								// Specify window position (at top left corner)
	win = glutCreateWindow("FFD (FYP-Elysia)");					// Set title of application window
	glClearColor(0.6, 0.6, 0.6, 0.5);

	/* Initialise callback functions */
	glutDisplayFunc(display);					// Register drawing function
	GLUI_Master.set_glutReshapeFunc(reshape);	// Register reshape function (need to use GLUI)
	glutMouseFunc(mouse);						// Register mouse function
	glutMotionFunc(motion);						// Register motion function
	glutKeyboardFunc(hotkey);					// Register keyboard funcion

	/* Setup OpenGL Lighting : LIGHT0 (White Light) */
	GLfloat light0_position[]	= {0.0f, 0.0f, 5.0f, 0.0f};				// Light position
	GLfloat light0_ambient[]	= {0.2f, 0.2f, 0.2f, 1.0f};				// Scene light
	GLfloat light0_diffuse[]	= {rLight0, gLight0, bLight0, 1.0f};	// Main color
	GLfloat light0_specular[]	= {1.0f, 1.0f, 1.0f, 1.0f};				// Highlights
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 70.0);

	/* Setup OpenGL Lighting : LIGHT1 */
	GLfloat light1_position[]	= {0.0f, 10.0f, 10.0f, 0.0f};	// Light position
	GLfloat light1_ambient[]	= {0.5f, 0.288f, 0.08f, 1.0f};	// Scene light
	GLfloat light1_diffuse[]	= {rLight1, gLight1, bLight1};	// Main color
	GLfloat light1_specular[]	= {1.0f, 1.0f, 1.0f, 1.0f};		// Highlights
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);

	setupGlui(win);	// Setup glui user interface
	glutMainLoop();	// Start glut main loop

	return 1;
}