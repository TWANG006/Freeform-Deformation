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
#include "GluiSetup.h"
#include "Mesh.h"
#include "Ffd.h"

// Camera variables
extern float translateX = 0, translateY = 0;
extern float angleX = -20.0, angleY = 20.0;
extern float scaleSize = 0.8;

// Object transformation variables
extern float translate[] = {0.0,0.0,0.0};
extern float rotationMatrix[16] = { 1.0,0.0,0.0,0.0,
									0.0,1.0,0.0,0.0,
									0.0,0.0,1.0,0.0,
									0.0,0.0,0.0,1.0 };

// Render variables
extern float color[] = {1.0, 1.0, 1.0, 0.5};
extern GLfloat rLight0 = 0.5f;
extern GLfloat gLight0 = 0.5f;
extern GLfloat bLight0 = 0.5f;
extern GLfloat rLight1 = 1.0f;
extern GLfloat gLight1 = 0.576f;
extern GLfloat bLight1 = 0.161f;

// GLUI bottom window variables
GLUI			*gluiBwin;
GLUI_Listbox	*colorListbox;
GLUI_Panel		*colorPanel;
GLUI_Panel		*renderPanel;
GLUI_Panel		*lightPanel;
extern GLUI_RadioGroup	*rModeGroup   = NULL;
extern GLUI_RadioGroup	*shadingGroup = NULL;
extern GLUI_RadioGroup	*projGroup	  = NULL;
extern GLUI_Checkbox	*axesChkbox	  = NULL;
extern GLUI_Checkbox	*bboxChkbox   = NULL;
extern GLUI_Checkbox	*fogChkbox	  = NULL;
extern GLUI_Checkbox	*transpChkbox = NULL;
extern GLUI_Checkbox	*light0Chkbox = NULL;
extern GLUI_Checkbox	*light1Chkbox = NULL;
GLUI_Checkbox	*sColorChkbox;
GLUI_Spinner	*redSpinner;
GLUI_Spinner	*greenSpinner;
GLUI_Spinner	*blueSpinner;
extern GLUI_Spinner *rLight0Spinner = NULL;
extern GLUI_Spinner *gLight0Spinner = NULL;
extern GLUI_Spinner *bLight0Spinner = NULL;
extern GLUI_Spinner *rLight1Spinner = NULL;
extern GLUI_Spinner *gLight1Spinner = NULL;
extern GLUI_Spinner *bLight1Spinner = NULL;

// GLUI left window variables
GLUI			*gluiLwin;
GLUI_Panel		*namePanel;
GLUI_Panel		*guidePanel;
GLUI_Panel		*meshPanel;
GLUI_Panel		*ffdPanel;
GLUI_Rollout	*sMeshRollout;
extern GLUI_RadioGroup	*sMeshGroup   = NULL;
extern GLUI_Spinner		*lattLSpinner = NULL;
extern GLUI_Spinner		*lattMSpinner = NULL;
extern GLUI_Spinner		*lattNSpinner = NULL;
extern GLUI_Checkbox	*selectAreaChkbox = NULL;
extern GLUI_Checkbox	*selectLattChkbox = NULL;

// GLUI right window variables
GLUI				*gluiRwin;
GLUI_Panel			*transformPanel;
GLUI_Panel			*translatePanel;
GLUI_Panel			*rotatePanel;
GLUI_Rotation		*rotation;
GLUI_Spinner		*scaling;
GLUI_Translation	*xyTranslate;
GLUI_Translation	*xTranslate;
GLUI_Translation	*yTranslate;
GLUI_Translation	*zTranslate;

// GLUI live variables
extern float ID_SCALE = 1.0f;
extern int	ID_AXES = 1;
extern int	ID_BBOX = 0;
extern int	ID_FOG = 0;
extern int	ID_TRANSP = 0;
extern int	ID_SMESH = -1;
extern int	ID_RMODE = 0;
extern int	ID_SHADE = 0;
extern int	ID_PROJ = 0;
extern int	ID_COLOR = 12;
static int	ID_COLOR_TEMP = 12;
static int	ID_SCOLOR = 0;
static int	ID_RED = 0;
static int	ID_GREEN = 0;
static int	ID_BLUE = 0;
extern int	ID_LIGHT0 = 1;
extern int	ID_LIGHT1 = 0;
static int	ID_RLIGHT0 = 0;
static int	ID_GLIGHT0 = 0;
static int	ID_BLIGHT0 = 0;
static int	ID_RLIGHT1 = 0;
static int	ID_GLIGHT1 = 0;
static int	ID_BLIGHT1 = 0;
extern int	ID_L = 0;
extern int	ID_M = 0;
extern int	ID_N = 0;
extern int	ID_SELECT_AREA = 0;
extern int	ID_SELECT_LATT = 0;
extern bool	sCol = false;

// GLUI IDs
enum
{
	BTN_CLEAR = 0,
	BTN_OPEN,
	RADIO_SMESH,
	RADIO_RMODE,
	RADIO_SHADE,
	RADIO_PROJ,
	CHKBOX_AXES,
	CHKBOX_BBOX,
	CHKBOX_FOG,
	CHKBOX_TRANSP,
	CHKBOX_LIGHT0,
	CHKBOX_LIGHT1,
	CHKBOX_SELECT_AREA,
	CHKBOX_SELECT_LATT,
	CHKBOX_SCOLOR,
	LISTBOX_COLOR,
	SPIN_RED,
	SPIN_GREEN,
	SPIN_BLUE,
	SPIN_RLIGHT0,
	SPIN_GLIGHT0,
	SPIN_BLIGHT0,
	SPIN_RLIGHT1,
	SPIN_GLIGHT1,
	SPIN_BLIGHT1,
	SPIN_L,
	SPIN_M,
	SPIN_N,
	SPIN_SCALE,
	TRANSLATE_XY,
	TRANSLATE_X,
	TRANSLATE_Y,
	TRANSLATE_Z,
	ROTATE,
	BTN_QUIT
};

// Get mesh variables
extern int numOfVert;
extern int numOfFace;
extern int numOfEdge;
extern bool haveMesh;
extern bool fileHaveNorm;

// Get bounding box variables
extern bool	 minMaxFlag;
extern float maxX, maxY, maxZ;
extern float minX, minY, minZ;

void clearMesh()
{
	numOfVert = 0;
	numOfFace = 0;
	numOfEdge = 0;
	maxX = 0.0;
	maxY = 0.0;
	maxZ = 0.0;
	minX = 0.0;
	minY = 0.0;
	minZ = 0.0;
	haveMesh = false;
	minMaxFlag = false;
	fileHaveNorm = false;
	resetLattice();
}

void clearGlui()
{
	ID_AXES = 1;		// Enable axes
	ID_BBOX = 0;		// Disable bounding box
	ID_FOG = 0;			// Disable fog
	ID_TRANSP = 0;		// Disable transparency
	ID_SMESH = -1;		// No given mesh selected
	ID_RMODE = 1;		// Solid mode
	ID_SHADE = 0;		// Smooth shading
	ID_PROJ = 0;		// Orthogonal projection
	ID_LIGHT0 = 1;		// Enable Light0
	ID_LIGHT1 = 0;		// Disable Light1
	ID_SELECT_AREA = 0;	// Disable selection (area)
	ID_SELECT_LATT = 0;	// Disable selection (lattice)
}

void clearView()
{
	/* Scene transformation */
	translateX = translateY = 0;
	angleX = -20.0; angleY = 20.0;
	scaleSize = 0.8;
	
	/* Object transformation */
	translate[0] = translate[1] = translate[2] = 0.0;
	rotationMatrix[0]  = rotationMatrix[5]  = rotationMatrix[10] = rotationMatrix[15] = 1.0f;
	rotationMatrix[1]  = rotationMatrix[2]  = rotationMatrix[3]  = rotationMatrix[4]  = 0.0f;
	rotationMatrix[6]  = rotationMatrix[7]  = rotationMatrix[8]  = rotationMatrix[9]  = 0.0f;
	rotationMatrix[11] = rotationMatrix[12] = rotationMatrix[13] = rotationMatrix[14] = 0.0f;
	ID_SCALE = 1.0f;
}

void reset()
{
	clearMesh();
	clearGlui();
	clearView();
}

void gluiCallback(int gluiControl)
{
	switch(gluiControl)
	{
		case BTN_CLEAR:
			cout << "BTN: Clear mesh\n";
			reset();
			break;
		case BTN_OPEN:
			cout << "BTN: Open mesh\n";
			createFileDialog();
			break;
		case RADIO_SMESH:
			clearMesh();
			haveMesh = true;
			switch(ID_SMESH) {
				case 0:
					cout << "RADIOBTN: Mesh bimba\n";
					loadMesh("bimba.m"); break;
				case 1:
					cout << "RADIOBTN: Mesh bottle\n";
					loadMesh("bottle.m"); break;
				case 2:
					cout << "RADIOBTN: Mesh bunny\n";
					loadMesh("bunny.m"); break;
				case 3:
					cout << "RADIOBTN: Mesh eight\n";
					loadMesh("eight.m"); break;
				case 4:
					cout << "RADIOBTN: Mesh gargoyle\n";
					loadMesh("gargoyle.m"); break;
				case 5:
					cout << "RADIOBTN: Mesh knot\n";
					loadMesh("knot.m"); break;
				case 6:
					cout << "RADIOBTN: Mesh statute\n";
					loadMesh("statute.m"); break;
			} 
			if(!fileHaveNorm) {
				calcFaceNorm();
				calcVertNorm();
			}
			setToOrigin();
			scaleToOrigin();
			resetLattice();
			initFFD(maxX, maxY, maxZ, minX, minY, minZ);
			break;
		case RADIO_RMODE:
			switch(ID_RMODE) {
				case 0:
					cout << "RADIOBTN: Rendering wireframe\n";
					break;
				case 1:
					cout << "RADIOBTN: Rendering solid\n";
					break;
				case 2:
					cout << "RADIOBTN: Rendering edge\n";
					break;
				case 3:
					cout << "RADIOBTN: Rendering point\n";
					break;
			} break;
		case RADIO_SHADE:
			switch(ID_SHADE) {
				case 0:
					cout << "RADIOBTN: Shading smooth\n";
					break;
				case 1:
					cout << "RADIOBTN: Shading flat\n";
					break;
			} break;
		case RADIO_PROJ:
			switch(ID_PROJ) {
				case 0:
					cout << "RADIOBTN: Projecting orthogonal\n";
					break;
				case 1:
					cout << "RADIOBTN: Projecting perspective\n";
					break;
			} break;
		case CHKBOX_AXES:
			switch(ID_AXES) {
				case 0:
					cout << "CHKBOX: Disable axes\n";
					break;
				case 1:
					cout << "CHKBOX: Enable axes\n";
					break;
			} break;
		case CHKBOX_BBOX:
			if(haveMesh) {
				switch(ID_BBOX) {
					case 0:
						cout << "CHKBOX: Disable bbox\n";
						break;
					case 1:
						cout << "CHKBOX: Enable bbox\n";
						break;
				}
			} else {
				// If there is no mesh
				if(createMsgbox(ERR_no_mesh) == IDOK) {
					createFileDialog();
					if(!haveMesh)
						bboxChkbox->set_int_val(0);
				} else {
					bboxChkbox->set_int_val(0);
				}
			} break;
		case CHKBOX_FOG:
			switch(ID_FOG) {
				case 0:
					cout << "CHKBOX: Disable fog\n";
					break;
				case 1:
					cout << "CHKBOX: Enable fog\n";
					break;
			} break;
		case CHKBOX_TRANSP:
			switch(ID_TRANSP) {
				case 0:
					cout << "CHKBOX: Disable transparent\n";
					break;
				case 1:
					cout << "CHKBOX: Enable transparent\n";
					break;
			} break;
		case CHKBOX_LIGHT0:
			switch(ID_LIGHT0) {
				case 0:
					cout << "CHKBOX: Disable light0\n";
					break;
				case 1:
					cout << "CHKBOX: Enable light0\n";
					break;
			} break;
		case CHKBOX_LIGHT1:
			switch(ID_LIGHT1) {
				case 0:
					cout << "CHKBOX: Disable light1\n";
					break;
				case 1:
					cout << "CHKBOX: Enable light1\n";
					break;
			} break;
		case CHKBOX_SELECT_AREA:
			if(haveMesh) {
				switch(ID_SELECT_AREA) {
					case 0:
						cout << "CHKBOX: Disable select area\n";
						break;
					case 1:
						cout << "CHKBOX: Enable select area\n";
						break;
				}
			} else {
				// If there is no mesh
				if(createMsgbox(ERR_no_mesh) == IDOK) {
					createFileDialog();
					if(!haveMesh)
						selectAreaChkbox->set_int_val(0);
				} else {
					selectAreaChkbox->set_int_val(0);
				}
			} break;
		case CHKBOX_SELECT_LATT:
			if(haveMesh) {
				switch(ID_SELECT_LATT) {
					case 0:
						cout << "CHKBOX: Disable lattice\n";
						break;
					case 1:
						cout << "CHKBOX: Enable lattice\n";
						break;
				}
			} else {
				// If there is no mesh
				if(createMsgbox(ERR_no_mesh) == IDOK) {
					createFileDialog();
					if(!haveMesh)
						selectLattChkbox->set_int_val(0);
				} else {
					selectLattChkbox->set_int_val(0);
				}
			} break;
		case CHKBOX_SCOLOR:
			switch(ID_SCOLOR) {
				case 0:
					cout << "CHKBOX: Disable special color\n";
					ID_COLOR = ID_COLOR_TEMP;
					sCol = false;
					break;
				case 1:
					cout << "CHKBOX: Enable special color\n";
					ID_COLOR_TEMP = ID_COLOR;
					colorListbox->set_int_val(0);
					sCol = true;
					break;
			} break;
		case LISTBOX_COLOR:
			sCol = false;
			sColorChkbox->set_int_val(0);
			switch(ID_COLOR) {
				case 0:
					cout << "LISTBOX: Color choose\n";
					break;
				case 1:
					cout << "LISTBOX: Color black\n";
					color[0] = color[1] = color[2] = 2.55/255.0;
					break;
				case 2:
					cout << "LISTBOX: Color blue\n";
					color[0] = color[1] = 2.55/255.0; color[2] = 255.0/255.0;
					break;
				case 3:
					cout << "LISTBOX: Color cyan\n";
					color[0] = 0.0/255.0; color[1] = color[2] = 255.0/255.0;
					break;
				case 4:
					cout << "LISTBOX: Color dark gray\n";
					color[0] = color[1] = color[2] = 64.0/255.0;
					break;
				case 5:
					cout << "LISTBOX: Color green\n";
					color[0] = color[2] = 0.0/255.0; color[1] = 255.0/255.0;
					break;
				case 6:
					cout << "LISTBOX: Color gray\n";
					color[0] = color[1] = color[2] = 128.0/255.0;
					break;
				case 7:
					cout << "LISTBOX: Color light gray\n";
					color[0] = color[1] = color[2] = 192.0/255.0;
					break;
				case 8:
					cout << "LISTBOX: Color magenta\n";
					color[0] = color[2] = 192.0/255.0; color[1] = 64.0/255.0;
					break;
				case 9:
					cout << "LISTBOX: Color orange\n";
					color[0] = 255.0/255.0; color[1] = 127.5/255.0; color[2] = 64.0/255.0;
					break;
				case 10:
					cout << "LISTBOX: Color pink\n";
					color[0] = color[2] = 255.0/255.0; color[1] = 0.0/255.0;
					break;
				case 11:
					cout << "LISTBOX: Color red\n";
					color[0] = 255.0/255.0; color[1] = color[2] = 0.0/255.0;
					break;
				case 12:
					cout << "LISTBOX: Color white\n";
					color[0] = color[1] = color[2] = 255.0/255.0;
					break;
				case 13:
					cout << "Color: yellow\n";
					color[0] = color[1] = 255.0/255.0; color[2] = 0.0/255.0;
					break;
			} 
			redSpinner	->set_float_val(color[0]);
			greenSpinner->set_float_val(color[1]);
			blueSpinner	->set_float_val(color[2]);
			break;
		case SPIN_RED:
		case SPIN_GREEN:
		case SPIN_BLUE:
			cout << "SPINNER: Color red/green/blue\n";
			sCol = false; sColorChkbox->set_int_val(0);
			color[0] = redSpinner->get_float_val();
			color[1] = greenSpinner->get_float_val();
			color[2] = blueSpinner->get_float_val();
			break;
		case SPIN_RLIGHT0:
			cout << "SPINNER: Light0 red\n";
			rLight0 = rLight0Spinner->get_float_val();
			break;
		case SPIN_GLIGHT0:
			cout << "SPINNER: Light0 green\n";
			gLight0 = gLight0Spinner->get_float_val();
			break;
		case SPIN_BLIGHT0:
			cout << "SPINNER: Light0 blue\n";
			bLight0 = bLight0Spinner->get_float_val();
			break;
		case SPIN_RLIGHT1:
			cout << "SPINNER: Light1 red\n";
			rLight1 = rLight1Spinner->get_float_val();
			break;
		case SPIN_GLIGHT1:
			cout << "SPINNER: Light1 green\n";
			gLight1 = gLight1Spinner->get_float_val();
			break;
		case SPIN_BLIGHT1:
			cout << "SPINNER: Light1 blue\n";
			bLight1 = bLight1Spinner->get_float_val();
			break;
		case SPIN_SCALE:
		case TRANSLATE_XY:
		case TRANSLATE_X:
		case TRANSLATE_Y:
		case TRANSLATE_Z:
		case ROTATE:
			cout << "OBJECT TRANFORMATION\n";
			break;
		case BTN_QUIT:
			cout << "BTN: Quit\n";
			exit(1);
			break;
	}
	GLUI_Master.sync_live_all();	// Syncs the button UI
	glutPostRedisplay();			// Redraw
}

void setupGlui(int win)
{
	setupBotWin(win);
	setupLeftWin(win);
	setupRightWin(win);
}

void setupBotWin(int win)
{
	gluiBwin = GLUI_Master.create_glui_subwindow(win, GLUI_SUBWINDOW_BOTTOM);
	gluiBwin ->set_main_gfx_window(win);

	/* Panel - Color */
	colorPanel = new GLUI_Panel(gluiBwin, "Color Options");
	new GLUI_StaticText(colorPanel, " ");
	// Choose color by listbox
	new GLUI_StaticText(colorPanel, "Default Colors:");
	colorListbox = new GLUI_Listbox(colorPanel, "", &ID_COLOR, LISTBOX_COLOR, gluiCallback);
	colorListbox->add_item(0,  "Choose Color");
	colorListbox->add_item(1,  "Black");
	colorListbox->add_item(2,  "Blue");
	colorListbox->add_item(3,  "Cyan");
	colorListbox->add_item(4,  "Dark Gray");
	colorListbox->add_item(5,  "Green");
	colorListbox->add_item(6,  "Gray");
	colorListbox->add_item(7,  "Light Gray");
	colorListbox->add_item(8,  "Magenta");
	colorListbox->add_item(9,  "Orange");
	colorListbox->add_item(10, "Pink");
	colorListbox->add_item(11, "Red");
	colorListbox->add_item(12, "White");
	colorListbox->add_item(13, "Yellow");
	colorListbox->set_int_val(12);	// Set default listbox color to be white
	// Special color
	sColorChkbox = new GLUI_Checkbox(colorPanel, "Special Color", &ID_SCOLOR, CHKBOX_SCOLOR, gluiCallback);
	gluiBwin->add_column_to_panel(colorPanel, true);
	// Choose color by spinners
	new GLUI_StaticText(colorPanel, "Select Colors:");
	redSpinner	 = gluiBwin->add_spinner_to_panel(colorPanel, "   Red", GLUI_SPINNER_FLOAT, &ID_RED, SPIN_RED, gluiCallback);
	greenSpinner = gluiBwin->add_spinner_to_panel(colorPanel, "Green", GLUI_SPINNER_FLOAT, &ID_GREEN, SPIN_GREEN, gluiCallback);
	blueSpinner	 = gluiBwin->add_spinner_to_panel(colorPanel, "  Blue", GLUI_SPINNER_FLOAT, &ID_BLUE, SPIN_BLUE, gluiCallback);
	redSpinner	 ->set_float_limits(0, 1, GLUI_LIMIT_CLAMP);
	greenSpinner ->set_float_limits(0, 1, GLUI_LIMIT_CLAMP);
	blueSpinner	 ->set_float_limits(0, 1, GLUI_LIMIT_CLAMP);

	/* Panel - Rendering */
	gluiBwin->add_column(false);
	renderPanel = new GLUI_Panel(gluiBwin, "Render Options");
	renderPanel->set_alignment(GLUI_ALIGN_CENTER);
	new GLUI_StaticText(renderPanel, " ");
	// Orthogonal/Perspective projection
	new GLUI_StaticText(renderPanel, "Projection:");
	projGroup = new GLUI_RadioGroup(renderPanel, &ID_PROJ, RADIO_PROJ, gluiCallback);
	new GLUI_RadioButton(projGroup, "Orthogonal (-)");
	new GLUI_RadioButton(projGroup, "Perspective (=)");
	gluiBwin->add_column_to_panel(renderPanel, true);
	new GLUI_StaticText(renderPanel, " ");
	// Smooth/Flat shading
	new GLUI_StaticText(renderPanel, "Shading:");
	shadingGroup = new GLUI_RadioGroup(renderPanel, &ID_SHADE, RADIO_SHADE, gluiCallback);
	new GLUI_RadioButton(shadingGroup, "Smooth ([)");
	new GLUI_RadioButton(shadingGroup, "Flat (])");
	gluiBwin->add_column_to_panel(renderPanel, true);
	// Rendering mode
	new GLUI_StaticText(renderPanel, "Rendering Mode:");
	rModeGroup = new GLUI_RadioGroup(renderPanel, &ID_RMODE, RADIO_RMODE, gluiCallback);
	new GLUI_RadioButton(rModeGroup, "Wireframe (W)");
	new GLUI_RadioButton(rModeGroup, "Solid (S)");
	new GLUI_RadioButton(rModeGroup, "Edge (E)");
	new GLUI_RadioButton(rModeGroup, "Point (P)");
	rModeGroup->set_int_val(1);
	gluiBwin  ->add_column_to_panel(renderPanel, true);
	// Other options
	new GLUI_StaticText(renderPanel, "Other Options:");
	axesChkbox	 = new GLUI_Checkbox(renderPanel, "Show Axes (A)", &ID_AXES, CHKBOX_AXES, gluiCallback);
	bboxChkbox	 = new GLUI_Checkbox(renderPanel, "Bounding Box (B)", &ID_BBOX, CHKBOX_BBOX, gluiCallback);
	fogChkbox	 = new GLUI_Checkbox(renderPanel, "Fog (F)", &ID_FOG, CHKBOX_FOG, gluiCallback);
	transpChkbox = new GLUI_Checkbox(renderPanel, "Transparent (T)", &ID_TRANSP, CHKBOX_TRANSP, gluiCallback);

	/* Panel - Lighting */
	gluiBwin->add_column(false);
	lightPanel = new GLUI_Panel(gluiBwin, "Lighting Options");
	lightPanel->set_alignment(GLUI_ALIGN_CENTER);
	new GLUI_StaticText(lightPanel, " ");
	// Light0/Light1
	new GLUI_StaticText(lightPanel, "Select light:");
	light0Chkbox = new GLUI_Checkbox(lightPanel, "Light 0 (0)", &ID_LIGHT0, CHKBOX_LIGHT0, gluiCallback);
	light1Chkbox = new GLUI_Checkbox(lightPanel, "Light 1 (1)", &ID_LIGHT1, CHKBOX_LIGHT1, gluiCallback);
	gluiBwin->add_column_to_panel(lightPanel, true);
	// Light0 color
	new GLUI_StaticText(lightPanel, "Light 0:");
	rLight0Spinner = gluiBwin->add_spinner_to_panel(lightPanel, "     Red Light", GLUI_SPINNER_FLOAT, &ID_RLIGHT0, SPIN_RLIGHT0, gluiCallback);
	gLight0Spinner = gluiBwin->add_spinner_to_panel(lightPanel, "  Green Light", GLUI_SPINNER_FLOAT, &ID_GLIGHT0, SPIN_GLIGHT0, gluiCallback);
	bLight0Spinner = gluiBwin->add_spinner_to_panel(lightPanel, "    Blue Light", GLUI_SPINNER_FLOAT, &ID_BLIGHT0, SPIN_BLIGHT0, gluiCallback);
	rLight0Spinner ->set_float_limits(0, 1, GLUI_LIMIT_CLAMP);
	gLight0Spinner ->set_float_limits(0, 1, GLUI_LIMIT_CLAMP);
	bLight0Spinner ->set_float_limits(0, 1, GLUI_LIMIT_CLAMP);
	gluiBwin->add_column_to_panel(lightPanel, true);
	// Light1 color
	new GLUI_StaticText(lightPanel, "Light 1:");
	rLight1Spinner = gluiBwin->add_spinner_to_panel(lightPanel, "     Red Light", GLUI_SPINNER_FLOAT, &ID_RLIGHT1, SPIN_RLIGHT1, gluiCallback);
	gLight1Spinner = gluiBwin->add_spinner_to_panel(lightPanel, "  Green Light", GLUI_SPINNER_FLOAT, &ID_GLIGHT1, SPIN_GLIGHT1, gluiCallback);
	bLight1Spinner = gluiBwin->add_spinner_to_panel(lightPanel, "    Blue Light", GLUI_SPINNER_FLOAT, &ID_BLIGHT1, SPIN_BLIGHT1, gluiCallback);
	rLight1Spinner ->set_float_limits(0, 1, GLUI_LIMIT_CLAMP);
	gLight1Spinner ->set_float_limits(0, 1, GLUI_LIMIT_CLAMP);
	bLight1Spinner ->set_float_limits(0, 1, GLUI_LIMIT_CLAMP);
}

void setupLeftWin(int win)
{
	gluiLwin = GLUI_Master.create_glui_subwindow(win, GLUI_SUBWINDOW_LEFT);
	gluiLwin ->set_main_gfx_window(win);

	/* Panel - Name */
	namePanel = new GLUI_Panel(gluiLwin, "CPE479: FYP");
	namePanel ->set_alignment(GLUI_ALIGN_CENTER);
	new GLUI_StaticText(namePanel, "Name: Ong Wei Xi, Elysia");
	new GLUI_StaticText(namePanel, "Matric. No.: U1022579E");

	/* Panel - Guidance */
	guidePanel = new GLUI_Panel(gluiLwin, "Under the guidance of");
	namePanel  ->set_alignment(GLUI_ALIGN_CENTER);
	new GLUI_StaticText(guidePanel, "Assistant Prof. Qian Kemao");
	new GLUI_StaticText(guidePanel, "Ph.D Student Wang Tianyi");

	/* Panel - Mesh */
	meshPanel = new GLUI_Panel(gluiLwin, "Select mesh");
	meshPanel ->set_alignment(GLUI_ALIGN_CENTER);
	// Clear mesh
	new GLUI_Button(meshPanel, "Clear Mesh (C)", BTN_CLEAR, gluiCallback);
	new GLUI_Separator(meshPanel);
	// Open mesh
	new GLUI_Button(meshPanel, "Open Mesh (O)", BTN_OPEN, gluiCallback);
	new GLUI_Separator(meshPanel);
	// Given mesh
	sMeshRollout = new GLUI_Rollout(meshPanel, "Specific Mesh");
	sMeshGroup	 = new GLUI_RadioGroup(sMeshRollout, &ID_SMESH, RADIO_SMESH, gluiCallback);
	new GLUI_RadioButton(sMeshGroup, "Bimba");
	new GLUI_RadioButton(sMeshGroup, "Bottle");
	new GLUI_RadioButton(sMeshGroup, "Bunny");
	new GLUI_RadioButton(sMeshGroup, "Eight");
	new GLUI_RadioButton(sMeshGroup, "Gargoyle");
	new GLUI_RadioButton(sMeshGroup, "Knot");
	new GLUI_RadioButton(sMeshGroup, "Statute");

	/* Panel - FFD */
	ffdPanel = new GLUI_Panel(gluiLwin, "Free-Form Deformation");
	ffdPanel ->set_alignment(GLUI_ALIGN_CENTER);
	// Select no. of planes
	lattLSpinner = gluiLwin->add_spinner_to_panel(ffdPanel, "S direction:  l", GLUI_SPINNER_INT, &ID_L, SPIN_L, gluiCallback);
	lattMSpinner = gluiLwin->add_spinner_to_panel(ffdPanel, "T direction: m", GLUI_SPINNER_INT, &ID_M, SPIN_M, gluiCallback);
	lattNSpinner = gluiLwin->add_spinner_to_panel(ffdPanel, "U direction: n", GLUI_SPINNER_INT, &ID_N, SPIN_N, gluiCallback);
	lattLSpinner->set_int_limits(1, 3, GLUI_LIMIT_CLAMP);
	lattMSpinner->set_int_limits(1, 3, GLUI_LIMIT_CLAMP);
	lattNSpinner->set_int_limits(1, 3, GLUI_LIMIT_CLAMP);
	lattLSpinner->set_int_val(3);
	lattMSpinner->set_int_val(3);
	lattNSpinner->set_int_val(3);
	new GLUI_Separator(ffdPanel);
	// Start FFD process
	selectAreaChkbox = new GLUI_Checkbox(ffdPanel, "Select Area (K)", &ID_SELECT_AREA, CHKBOX_SELECT_AREA, gluiCallback);
	selectLattChkbox = new GLUI_Checkbox(ffdPanel, "Select Lattice (L)", &ID_SELECT_LATT, CHKBOX_SELECT_LATT, gluiCallback);

	/* Quit */
	gluiLwin->add_button("Quit (Q)", BTN_QUIT, gluiCallback);
}

void setupRightWin(int win)
{
	gluiRwin = GLUI_Master.create_glui_subwindow(win, GLUI_SUBWINDOW_RIGHT);
	gluiRwin ->set_main_gfx_window(win);

	/* Panel - Transformation */
	transformPanel = new GLUI_Panel(gluiRwin, "Object Transformation:");
	// Translation
	translatePanel = gluiRwin->add_panel_to_panel(transformPanel, "");
	translatePanel ->set_alignment(GLUI_ALIGN_CENTER);
	// XY-translation
	xyTranslate = gluiRwin->add_translation_to_panel(translatePanel, "Translate XY", GLUI_TRANSLATION_XY, translate, TRANSLATE_XY, gluiCallback);
	xyTranslate ->set_speed(.005);
	new GLUI_Separator(translatePanel);
	// X-translation
	xTranslate = gluiRwin->add_translation_to_panel(translatePanel, "Translate X", GLUI_TRANSLATION_X, &translate[0], TRANSLATE_X, gluiCallback);
	xTranslate ->set_speed(.005);
	new GLUI_Separator(translatePanel);
	// Y-translation
	yTranslate = gluiRwin->add_translation_to_panel(translatePanel, "Translate Y", GLUI_TRANSLATION_Y, &translate[1], TRANSLATE_Y, gluiCallback);
	yTranslate ->set_speed(.005);
	new GLUI_Separator(translatePanel);
	// Z-translation
	zTranslate = gluiRwin->add_translation_to_panel(translatePanel, "Translate Z", GLUI_TRANSLATION_Z, &translate[2], TRANSLATE_Z, gluiCallback);
	zTranslate->set_speed(.005);
	new GLUI_Separator(transformPanel);
	// Rotation
	rotatePanel = gluiRwin->add_panel_to_panel(transformPanel, "");
	rotatePanel ->set_alignment(GLUI_ALIGN_CENTER);
	rotation = gluiRwin->add_rotation_to_panel(rotatePanel, "Rotation", rotationMatrix, ROTATE, gluiCallback);
	rotation ->set_spin(1.0);
	new GLUI_Separator(transformPanel);
	// Scaling
	scaling = new GLUI_Spinner(transformPanel, "Scale ", &ID_SCALE, SPIN_SCALE, gluiCallback);
	scaling ->set_float_limits(-4.0,4.0);
}