#pragma comment( linker, "/subsystem:windows /entry:mainCRTStartup" )
#define GLUT_DISABLE_ATEXIT_HACK

#include<ctime>
#include<Windows.h>
#include<iostream>
#include<stdio.h>
#include<map>
#include<math.h>
#include<GL/glui.h>
#include "MeshViewer.h"

using namespace std;

//GLUI controls 
#define IMPORT_ID			     	100
#define BACKGROUND_COLOR_ID			101
#define OBJECT_COLOR_ID				102
#define DISPLAY_ID					103
#define PROJECTION_ID				104
#define DISPLAY_POINTS_ID			200
#define DISPLAY_WIRE_ID				201
#define DISPLAY_FLAT_ID				202
#define DISPLAY_SMOOTH_ID			203
#define ORTHO_PROJECTION_ID			300
#define PERSP_PROJECTION_ID			301
#define TRANSFORM_ROTATION_ID		400
#define TRANSFORM_TRANSLATION_ID	401
#define TRANSFORM_SCALING_ID		402
#define TRANSFORM_NONE_ID			403
#define LIGHT0_ENABLE_ID			500
#define LIGHT1_ENABLE_ID			501
#define LIGHT0_INTENSITY_ID			550
#define LIGHT1_INTENSITY_ID			551
#define FFD_ID                      600
#define FFD_RESET_ID				601

//Global variables for reading the mesh
static HE_vert	*vertex, *v0;                   //Allocate memories for vertices, faces and edges. 
static HE_face	*face; 
static HE_edge	*edge; 
static HE_vert  top_vertex[4];				//Used to set bounding box
static HE_vert  bottom_vertex[4];
static HE_vert  ***FFD_pt;                  //Points used to do FFD
static GLdouble ***dWeight;						//Weights of FFD
static HE_vert  vec_S,vec_T,vec_U;          //Three directional vectors

static int i_vertex_num=0,i_face_num=0,i_edge_num = 0;
GLfloat xy_aspect, x_axis = 5.0f, y_axis = 5.0f, z_axis = 5.0f;
int load = 0, reload = 0;
int iDisplay_mode = DISPLAY_SMOOTH_ID;							 //Initialize the display mode to smooth	
int iProjection_mode = PERSP_PROJECTION_ID;						 //Initialize the projection mode to perspective
GLfloat glB_r = 0.9f, glB_g = 0.9f, glB_b = 0.9f;				 //The initial background color
GLfloat glObj_r = 0.75f, glObj_g = 0.0f, glObj_b = 0.75f;
GLUquadricObj *quadratic;

//OpenGL lights
GLfloat LightDiffuse [] = {1.0f,0.0f,1.0f,1.0f};    //Used to set the diffuse color of the object
GLfloat light0_ambient [] = {0.1f,0.1f,0.3f,1.0f};
GLfloat light0_diffuse [] = {0.6f,0.6f,1.0f,1.0f};
GLfloat light0_position [] = {0.5f,0.5f,1.0f,0.0f};
GLfloat light1_ambient [] = {0.1f,0.1f,0.3f,1.0f};
GLfloat light1_diffuse [] = {0.9f,0.6f,0.0f,1.0f};
GLfloat light1_position [] = {-1.0f,-1.0f,1.0f,0.0f};
GLfloat light0_rotation [16] = {1.0,0.0,0.0,0.0, 0.0,1.0,0.0,0.0, 0.0,0.0,1.0,0.0, 0.0,0.0,0.0,1.0};
GLfloat light1_rotation [16] = {1.0,0.0,0.0,0.0, 0.0,1.0,0.0,0.0, 0.0,0.0,1.0,0.0, 0.0,0.0,0.0,1.0};

//Global variables for GLUI
int main_win;
int iDraw_axes = 0;									//Not to draw the axes first
int iDraw_grid = 1;									//Draw grid when start
int iDraw_boundingbox = 0;							//Ensure if to draw the bounding box
int iDraw_edges = 0;								//Used to decide whether to draw edges.
int iObject = 0;									//Use mouse to control the transformation of the object only
int iLight0_enabled = 1;							//Two light sources are all enabled first
int iLight1_enabled = 1;

//Freeform deformation
#define BUFSIZE 512
int iS=1,iT=1,iU=1;                                 //Free Form Deformation factors
int old_iS =1, old_iT =1, old_iU =1;                //Strore the old factors
int grabCP_x = -1, grabCP_y = -1, grabCP_z = -1;
GLint viewport[4];
GLdouble mvmatrix[16], projmatrix[16];
GLint realy;
GLdouble wx, wy, wz;
GLdouble vx, vy, vz;
GLdouble cwx, cwy, cwz;
int  isPicking = 0;
bool isSelectedCP = false;

float fLight0_intensity = 1.0;
float fLight1_intensity = .4;
float fView_rotation [16] = {1.0,0.0,0.0,0.0, 0.0,1.0,0.0,0.0, 0.0,0.0,1.0,0.0, 0.0,0.0,0.0,1.0};
float fObj_position [] = {0.0,0.0,0.0};
GLUI *sub_window_right, *sub_window_bottom, *sub_window_left;
GLUI_Panel *panel_display_mode, *panel_projection_mode, *panel_import, *panel_scene_operation;
GLUI_RadioGroup *radio_group_display, *radio_group_projection;
GLUI_Spinner *light0_spinner, *light1_spinner, *S_spinner, *T_spinner, *U_spinner;
GLfloat x_rotate=-30.0f, y_rotate, x_dis=0.0f, y_dis=-1.0f, scale_scene = 0.5f, Ini_size;   //Variables used to rotate the scene
GLfloat obj_x_rotate=0.0f, obj_y_rotate=0.0f, obj_x_dis=0.0f, obj_y_dis=0.0f, scale_obj=1.0f;
int iMouse_x, iMouse_y;
int iTransform_mode;
float temp;

//All functions used in this viewer
//Reading file functions
void mesh_import(char *ch_filename) ;
void normalize( float v[3] );
void normcrossprod( float vector1[3],float vector2[3],float out[3] );
float compute_area(float vector1[3],float vector2[3]);
void compute_vert_norm(void);


//FFD
void computePijk(int l, int m, int n);
int i_factorial(int i_all, int i_i);
int factorial(int n);
void processHits(GLint hits, GLuint buffer[]);
void moveControlPt(int x, int y);
void picking(int button, int state, int x, int y);
void draw(GLenum mode);
void DeformMesh(HE_vert v[]);
void computeVert();

//opengl GLUI functions
void boundingbox();   //compute bounding box
void render_mesh();
static void GL_init(void);
void mouse_motion(int x, int y);
void mouse(int button, int state, int x, int y);
void control(int control);
void display(void);
void reshape(int w, int h);

//main function
int main(int argc, char* argv[])
{
	
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(280,200);
	glutInitWindowSize(1200,800);
	main_win = glutCreateWindow("Interactive Shape Editing. Created by Wang Tianyi");
	glutMouseFunc(mouse);
	glutMotionFunc(mouse_motion);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	GL_init();
	//Build GLUI environment RIGHT
	sub_window_right = GLUI_Master.create_glui("Operations",0L,0,200);
	//Button panel
	panel_import = new GLUI_Panel(sub_window_right,"",1);
	new GLUI_Button(panel_import,"Import Mesh", IMPORT_ID,control);
	new GLUI_Button(panel_import,"Background Color", BACKGROUND_COLOR_ID,control);
	new GLUI_Button(panel_import,"Object Color", OBJECT_COLOR_ID,control);

	//Display mode panel
	GLUI_Rollout *roll_displays = new GLUI_Rollout(sub_window_right, "Display Mode", true);
	new GLUI_StaticText(roll_displays, " ");
	panel_display_mode = new GLUI_Panel(roll_displays,"Choose the Display Mode",1);
	radio_group_display = new GLUI_RadioGroup(panel_display_mode,0,DISPLAY_ID,control);
	new GLUI_RadioButton( radio_group_display,"Points Cloud");
	new GLUI_RadioButton( radio_group_display,"Wireframe");
	new GLUI_RadioButton( radio_group_display,"Flat Shading");
	new GLUI_RadioButton( radio_group_display,"Smooth Shading");
	radio_group_display->set_int_val(3);
	//Projection mode panel
	new GLUI_StaticText(roll_displays," ");
	panel_projection_mode = new GLUI_Panel(roll_displays, "Choose Projection Mode",1);
	radio_group_projection = new GLUI_RadioGroup(panel_projection_mode,0,PROJECTION_ID,control);
	new GLUI_RadioButton(radio_group_projection,"ORTHOGONAL PROJECTION");
	new GLUI_RadioButton(radio_group_projection,"PERSPECTIVE PROJECTION");
	radio_group_projection->set_int_val(1);

	//Lights roll
	GLUI_Rollout *roll_lights = new GLUI_Rollout(sub_window_right, "Lights Operations", false);
	GLUI_Panel *light0 = new GLUI_Panel(roll_lights, "LIGHT 1");
	GLUI_Panel *light1 = new GLUI_Panel(roll_lights, "LIGHT 2");
	new GLUI_Checkbox(light0, "ENABLED", &iLight0_enabled, LIGHT0_ENABLE_ID,control);
	light0_spinner = new GLUI_Spinner(light0, "Intensity: ",&fLight0_intensity,LIGHT0_INTENSITY_ID,control);
	light0_spinner->set_float_limits(0.0f,1.0f);
	GLUI_Scrollbar *sb;
	sb = new GLUI_Scrollbar(light0,"RED",GLUI_SCROLL_HORIZONTAL,&light0_diffuse[0],LIGHT0_INTENSITY_ID,control);
	sb->set_float_limits(0.0f,1.0f);
	sb = new GLUI_Scrollbar(light0,"GREEN",GLUI_SCROLL_HORIZONTAL,&light0_diffuse[1],LIGHT0_INTENSITY_ID,control);
	sb->set_float_limits(0.0f,1.0f);
	sb = new GLUI_Scrollbar(light0,"BLUE",GLUI_SCROLL_HORIZONTAL,&light0_diffuse[2],LIGHT0_INTENSITY_ID,control);
	sb->set_float_limits(0.0f,1.0f);
	new GLUI_Checkbox(light1, "ENABLED", &iLight1_enabled, LIGHT1_ENABLE_ID,control);
	light1_spinner = new GLUI_Spinner(light1, "Intensity: ",&fLight1_intensity,LIGHT1_INTENSITY_ID,control);
	light1_spinner->set_float_limits(0.0f,1.0f);
	sb = new GLUI_Scrollbar(light1,"RED",GLUI_SCROLL_HORIZONTAL,&light1_diffuse[0],LIGHT1_INTENSITY_ID,control);
	sb->set_float_limits(0.0f,1.0f);
	sb = new GLUI_Scrollbar(light1,"GREEN",GLUI_SCROLL_HORIZONTAL,&light1_diffuse[1],LIGHT1_INTENSITY_ID,control);
	sb->set_float_limits(0.0f,1.0f);
	sb = new GLUI_Scrollbar(light1,"BLUE",GLUI_SCROLL_HORIZONTAL,&light1_diffuse[2],LIGHT1_INTENSITY_ID,control);
	sb->set_float_limits(0.0f,1.0f);
	new GLUI_StaticText(roll_lights," ");
	GLUI_Rotation *light0_rot = new GLUI_Rotation(roll_lights, "LIGHT0", light0_rotation);
	light0_rot->set_spin(1.0);
	new GLUI_StaticText(roll_lights," ");
	GLUI_Rotation *light1_rot = new GLUI_Rotation(roll_lights, "LIGHT1", light1_rotation);
	light1_rot->set_spin(1.0);

	//Choose the scene operations.
	GLUI_Rollout *roll_FFDs = new GLUI_Rollout(sub_window_right, "FFD", false);
	new GLUI_StaticText(roll_FFDs," ");
	panel_scene_operation = new GLUI_Panel(roll_FFDs," ",1);
	new GLUI_Checkbox(panel_scene_operation, "Draw Axes", &iDraw_axes);
	new GLUI_Checkbox(panel_scene_operation, "Draw Grid", &iDraw_grid);
	new GLUI_Checkbox(panel_scene_operation, "Show Control Points and Lattice", &iDraw_boundingbox);
	new GLUI_Checkbox(panel_scene_operation, "Draw Edges", &iDraw_edges);
	new GLUI_Checkbox(panel_scene_operation, "Play with the Mesh Itself", &iObject);
	new GLUI_Checkbox(panel_scene_operation, "Start Picking",&isPicking);
	new GLUI_Button(panel_scene_operation,"Reset Mesh", FFD_RESET_ID,control);
	//FFD panel
	new GLUI_StaticText(roll_FFDs," ");
	GLUI_Panel *FFD = new GLUI_Panel(roll_FFDs,"FFD Factors");
	S_spinner = new GLUI_Spinner(FFD,"l-factor",&iS,FFD_ID,control);
	S_spinner->set_int_limits(1,4);
	T_spinner = new GLUI_Spinner(FFD,"m-factor",&iT,FFD_ID,control);
	T_spinner->set_int_limits(1,4);
	U_spinner = new GLUI_Spinner(FFD,"n-factor",&iU,FFD_ID,control);
	U_spinner->set_int_limits(1,4);
	new GLUI_Button(FFD,"Reset Control Polygon", FFD_ID,control);
	
	GLUI_Rollout *roll_objects = new GLUI_Rollout(sub_window_right, "Object Transform", false);
	new GLUI_StaticText(roll_objects,"Transform the Object. ");
	GLUI_Rotation *view_rot = new GLUI_Rotation(roll_objects,"Object",fView_rotation);
	view_rot->set_spin(1.0);
	GLUI_Translation *trans_xy = new GLUI_Translation(roll_objects,"Object XY",GLUI_TRANSLATION_XY,fObj_position);
	trans_xy->set_speed(.005);
	GLUI_Translation *trans_x = new GLUI_Translation(roll_objects,"Object X",GLUI_TRANSLATION_X,&fObj_position[0]);
	trans_x->set_speed(.005);
	GLUI_Translation *trans_y = new GLUI_Translation(roll_objects,"Object Y",GLUI_TRANSLATION_Y,&fObj_position[1]);
	trans_y->set_speed(.005);
	GLUI_Translation *trans_z = new GLUI_Translation(roll_objects,"Object Z",GLUI_TRANSLATION_Z,&fObj_position[2]);
	trans_z->set_speed(.005);

	//Exit
	new GLUI_Button(sub_window_right,"EXIT", 0, (GLUI_Update_CB)exit);
	sub_window_right->set_main_gfx_window(main_win);

	glutMainLoop();
	return 0;
}
/* Funtions for reading file and building up Half-edge data structure, 
	computing the normal for each vertex.Take the boundary contitions into consideration.*/
void mesh_import(char *ch_filename)   
{
	FILE *file;                                      //Used to open file
    char ch_read;                                    //read characters from the .M file
	int i_vertex_index,i_face_index,edge_num=0,i_v1,i_v2,i_v3;  //Indices of vertices,faces; Three points of one face.
	float x,y,z;                                     //Coordinates of vertices.

	//map the half edges
	map<HE_line,int>edge_map;
	map<HE_line,int>::iterator edge_map_iterator;
	edge_map.clear();
	int vstart=0, vend=0;

	 clock_t start,  finish;   //typedef long clock_t; 
	 double totaltime;   
	 start = clock();   //clock():Current time of CPU   

	//Open file for the first time
	fopen_s(&file,ch_filename,"r");
	while((ch_read=fgetc(file))!= EOF)
	{
		switch (ch_read)
		{
		case 'V': if ( fscanf(file,"ertex %d %f %f %f \n",
					          &i_vertex_index,&x,&y,&z) == 4 )
				  {
				  }
				  break;
		case 'F': if ( fscanf(file,"ace %d %d %d %d\n",&i_face_index,&i_v1,&i_v2,&i_v3) == 4)
				  {
				  }
				  break;
				  // "#"indicates the commends lines. 
		case '#': do
				  { 
					  ch_read =fgetc(file);
				  }
				  while( ch_read != '\n' && ch_read != EOF);
				  break;
		}
	}
	i_vertex_num = i_vertex_index;
	i_face_num = i_face_index;

	vertex = new HE_vert[i_vertex_num]();
	v0 = new HE_vert[i_vertex_num];
	face = new HE_face[i_face_num]();
	edge = new HE_edge[3*i_face_num]();

	//open file for the second time
	fopen_s(&file,ch_filename,"r");
	while((ch_read=fgetc(file))!= EOF)
	{
		switch (ch_read)
		{
		case 'V': if ( fscanf(file,"ertex %d %f %f %f \n",
					          &i_vertex_index,&x,&y,&z) == 4 )
				  {
					  //store vertex information
					  vertex[i_vertex_index-1].x=(GLfloat)x;
					  vertex[i_vertex_index-1].y=(GLfloat)y;
					  vertex[i_vertex_index-1].z=(GLfloat)z;
					  vertex[i_vertex_index-1].v_index=i_vertex_index-1;
				  }
				  break;
		case 'F': if ( fscanf(file,"ace %d %d %d %d\n",&i_face_index,&i_v1,&i_v2,&i_v3) == 4)
				  {
					  //store face information
					  
					  face[i_face_index-1].v1=i_v1;
					  face[i_face_index-1].v2=i_v2;
					  face[i_face_index-1].v3=i_v3;
					  face[i_face_index-1].f_index = i_face_index-1;

					  //set edge information meanwhile find the pair domain of each edge.
					  //The first edge of a face
					  edge[(i_face_index-1)*3].vert_start = &vertex[face[i_face_index-1].v1-1];
					  edge[(i_face_index-1)*3].vert_end = &vertex[face[i_face_index-1].v2-1];	
					  vstart = vertex[face[i_face_index-1].v1-1].v_index;
					  vend = vertex[face[i_face_index-1].v2-1].v_index;
					  edge[(i_face_index-1)*3].vert_start->edge = &edge[(i_face_index-1)*3];
					  edge[(i_face_index-1)*3].e_index = (i_face_index-1)*3;
					  edge[(i_face_index-1)*3].face = &face[i_face_index-1];
					  edge_num++;

					  edge_map_iterator=edge_map.find(HE_line(vend,vstart));
					  if (edge_map_iterator!=edge_map.end())
					  {
						  edge[(i_face_index-1)*3].pair= &edge[edge_map_iterator->second-1];
						  edge[edge_map_iterator->second-1].pair = &edge[(i_face_index-1)*3];
						  edge_map.erase(edge_map_iterator);
					  }
					  else
					  {
						  edge_map.insert(pair<HE_line,int>(HE_line(vstart,vend),edge_num));
					  }

					  
					  //The second edge of a face
					  edge[(i_face_index-1)*3+1].vert_start = &vertex[face[i_face_index-1].v2-1];
					  edge[(i_face_index-1)*3+1].vert_end = &vertex[face[i_face_index-1].v3-1];
					  vstart = vertex[face[i_face_index-1].v2-1].v_index;
					  vend = vertex[face[i_face_index-1].v3-1].v_index;
					  edge[(i_face_index-1)*3+1].vert_start->edge = &edge[(i_face_index-1)*3+1];
					  edge[(i_face_index-1)*3+1].e_index = (i_face_index-1)*3+1;
					  edge[(i_face_index-1)*3+1].face = &face[i_face_index-1];
					  edge_num++;
					  edge_map_iterator=edge_map.find(HE_line(vend,vstart));

					  if (edge_map_iterator!=edge_map.end())
					  {
						  
						  edge[(i_face_index-1)*3+1].pair= &edge[edge_map_iterator->second-1];
						  edge[edge_map_iterator->second-1].pair = &edge[(i_face_index-1)*3+1];
						  edge_map.erase(edge_map_iterator);
					  }
					  else
					  {
						  edge_map.insert(pair<HE_line,int>(HE_line(vstart,vend),edge_num));
					  }


					  //The third edge of a face.
					  edge[(i_face_index-1)*3+2].vert_start = &vertex[face[i_face_index-1].v3-1];
					  edge[(i_face_index-1)*3+2].vert_end = &vertex[face[i_face_index-1].v1-1];
					  vstart = vertex[face[i_face_index-1].v3-1].v_index;
					  vend = vertex[face[i_face_index-1].v1-1].v_index;
					  edge[(i_face_index-1)*3+2].vert_start->edge = &edge[(i_face_index-1)*3+2];
					  edge[(i_face_index-1)*3+2].e_index = (i_face_index-1)*3+2;
					  edge[(i_face_index-1)*3+2].face = &face[i_face_index-1];
					  edge_num++;

					  //find pairs
					  edge_map_iterator=edge_map.find(HE_line(vend,vstart));
					  if (edge_map_iterator!=edge_map.end())
					  {
						  edge[(i_face_index-1)*3+2].pair= &edge[edge_map_iterator->second-1];
						  edge[edge_map_iterator->second-1].pair = &edge[(i_face_index-1)*3+2];
						  edge_map.erase(edge_map_iterator);
					  }
					  else
					  {
						  edge_map.insert(pair<HE_line,int>(HE_line(vstart,vend),edge_num));
					  }


					  //set the next domain
					  edge[(i_face_index-1)*3].next = &edge[(i_face_index-1)*3+1];
					  edge[(i_face_index-1)*3+1].next = &edge[(i_face_index-1)*3+2];
					  edge[(i_face_index-1)*3+2].next = &edge[(i_face_index-1)*3];

					  //set the face's HE_edge domain
					  face[i_face_index-1].edge = &edge[(i_face_index-1)*3];
				  }
				  break;
				  // "#"indicates the commends lines. 
		case '#': do
				  { 
					  ch_read =fgetc(file);
				  }
				  while( ch_read != '\n' && ch_read != EOF);
				  break;
		}
	}

	//Store the number of vertex and the number of face 
    i_vertex_num = i_vertex_index;
	i_face_num = i_face_index;
	i_edge_num  = edge_num;

	//Clear the half edge map.
	edge_map.clear();

	//deal with the boundaries 
	for(int n=0;n<i_edge_num;n++)
	{
		if(edge[n].pair == NULL)
			vertex[edge[n].vert_start->v_index].edge= &edge[n];
	}
	finish=clock();   
	totaltime=(double)(finish-start)/CLOCKS_PER_SEC;   
	cout<<"\nRuntime is: " << totaltime << "s" << endl;   
}
void normalize( float v[3] )
{
	float d = sqrtf( v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
	if ( d == 0 )
	{
		 cout<<"myerror-d"<<endl;
	  return;
	}

	v[0] /=d;
	v[1] /=d;
	v[2] /=d;
}
void normcrossprod( float vector1[3],float vector2[3],float out[3] )
{
	out[0] = vector1[1]*vector2[2]-vector1[2]*vector2[1];
	out[1] = vector1[2]*vector2[0]-vector1[0]*vector2[2];
	out[2] = vector1[0]*vector2[1]-vector1[1]*vector2[0];
	normalize( out );
}
float compute_area(float vector1[3],float vector2[3])
{
	float area;
	return area = sqrt((vector1[1]*vector2[2]-vector1[2]*vector2[1])*(vector1[1]*vector2[2]-vector1[2]*vector2[1])
						+(vector1[2]*vector2[0]-vector1[0]*vector2[2])*(vector1[2]*vector2[0]-vector1[0]*vector2[2])
						+(vector1[0]*vector2[1]-vector1[1]*vector2[0])*(vector1[0]*vector2[1]-vector1[1]*vector2[0]));
}
void compute_vert_norm(HE_vert vertex[])
{
	HE_edge *He, *curr;
	int j;
	float coord[3];
	float area[50];
	float d1[3],d2[3];
	float area_sum;
	static Normal_vector normal[50];
	for(int i=0;i<i_vertex_num;i++)
	{
		j=0; area_sum = 0.0;
		curr=vertex[i].edge;
		He = curr;

		if(He->pair == NULL)
		{
		do
		{
			j++;
			d1[0] = He->next->vert_start->x - He->vert_start->x;
			d1[1] = He->next->vert_start->y - He->vert_start->y;
			d1[2] = He->next->vert_start->z - He->vert_start->z;

			d2[0] = He->next->next->vert_start->x - He->vert_start->x;
			d2[1] = He->next->next->vert_start->y - He->vert_start->y;
			d2[2] = He->next->next->vert_start->z - He->vert_start->z;

			normcrossprod(d1,d2,normal[j-1].normal );
			area[j-1] = compute_area(d1,d2);
			area_sum+= area[j-1];

			He = He->next->next->pair;
		}
		while(He != NULL);
		}
		else
		{
		do
		{
			j++;
			d1[0] = He->next->vert_start->x - He->vert_start->x;
			d1[1] = He->next->vert_start->y - He->vert_start->y;
			d1[2] = He->next->vert_start->z - He->vert_start->z;

			d2[0] = He->next->next->vert_start->x - He->vert_start->x;
			d2[1] = He->next->next->vert_start->y - He->vert_start->y;
			d2[2] = He->next->next->vert_start->z - He->vert_start->z;

			normcrossprod(d1,d2,normal[j-1].normal );
			area[j-1] = compute_area(d1,d2);
			area_sum+= area[j-1];
			
			He = He->pair->next;
		}
		while(He != curr);

		}

		//compute the weight for each normal
		 coord[0] =0.0,coord[1]=0.0,coord[2]=0.0;
		for(int p=0;p<j;p++)
		{
			coord[0] += area[p]/area_sum * normal[p].normal[0];
			coord[1] += area[p]/area_sum * normal[p].normal[1];
			coord[2] += area[p]/area_sum * normal[p].normal[2];

			/*coord[0] += 1.0/j * normal[p].normal[0];
			coord[1] += 1.0/j * normal[p].normal[1];
			coord[2] += 1.0/j * normal[p].normal[2];*/		
		}
		normalize(coord);
		vertex[i].normal[0] = coord[0];
		vertex[i].normal[1] = coord[1];
		vertex[i].normal[2] = coord[2];
	}	
		
}

//Functions for rendering
void boundingbox()
{
	GLfloat glx_MX, gly_MX, glz_MX;
	GLfloat glx_MI, gly_MI, glz_MI;
	glx_MX = gly_MX = glz_MX = -10.0f;
	glx_MI = gly_MI = glz_MI = 10.0f;
	for(int i = 0; i<i_vertex_num; i++)
	{
		//The minimum of x is for the left-most vertices of the top and bottom vertices.(Arranged in CCW order)
		if(vertex[i].x<glx_MI)
		{
			top_vertex[0].x = vertex[i].x;
			top_vertex[3].x = vertex[i].x;
			bottom_vertex[0].x = vertex[i].x;
			bottom_vertex[3].x = vertex[i].x;
			glx_MI = vertex[i].x;
		}
		//The minimum of y is the smallest vertex of the four points of the bottom.
		if(vertex[i].y<gly_MI)
		{
			bottom_vertex[0].y = vertex[i].y;
			bottom_vertex[1].y = vertex[i].y;
			bottom_vertex[2].y = vertex[i].y;
			bottom_vertex[3].y = vertex[i].y;
			gly_MI = vertex[i].y;
		}
		//The minimum of z is obtained from the front surface.(Left-hand rule)
		if(vertex[i].z<glz_MI)
		{
			top_vertex[0].z = vertex[i].z;
			top_vertex[1].z = vertex[i].z;
			bottom_vertex[0].z = vertex[i].z;
			bottom_vertex[1].z = vertex[i].z;
			glz_MI = vertex[i].z;
		}
		//Find the corresponding max values
		if(vertex[i].x>glx_MX)
		{
			top_vertex[1].x = vertex[i].x;
			top_vertex[2].x = vertex[i].x;
			bottom_vertex[1].x = vertex[i].x;
			bottom_vertex[2].x = vertex[i].x;
			glx_MX = vertex[i].x;
		}
		if(vertex[i].y>gly_MX)
		{
			top_vertex[0].y = vertex[i].y;
			top_vertex[1].y = vertex[i].y;
			top_vertex[2].y = vertex[i].y;
			top_vertex[3].y = vertex[i].y;
			gly_MX = vertex[i].y;
		}
		if(vertex[i].z>glz_MX)
		{
			top_vertex[2].z = vertex[i].z;
			top_vertex[3].z = vertex[i].z;
			bottom_vertex[2].z = vertex[i].z;
			bottom_vertex[3].z = vertex[i].z;
			glz_MX = vertex[i].z;
		}
	}
	if(top_vertex[1].x>=top_vertex[2].z)
			temp = top_vertex[1].x;
		else
			temp = top_vertex[2].z;
		if(temp<=top_vertex[0].y)
			temp = top_vertex[0].y;
}
void settoorigin( HE_vert v[], HE_vert bottom_vertex[])
{
	int i;
	for(i=0;i<i_vertex_num;i++)
	{
		v[i].x = 1.5*(v[i].x - bottom_vertex[0].x)/temp;
		v[i].y = 1.5*(v[i].y - bottom_vertex[0].y)/temp;
		v[i].z = 1.5*(v[i].z - bottom_vertex[0].z)/temp;
	}
}

//FFD functions
void processHits(GLint hits, GLuint buffer[])
{
	unsigned int i, j;
	GLuint names, *ptr, picked;

	ptr = (GLuint*)buffer;
	for (i = 0; i < hits; i++) { /* for each hit */
    names = *ptr;
   cout<<names; 
	ptr++;
   printf(" z1 is %g;", (float) *ptr/0x7fffffff); 
	ptr++;
    printf(" z2 is %g\n", (float) *ptr/0x7fffffff); 
	ptr++;
    printf(" the name is ");
    for(j = 0; j < names; j++) { /* for each name */
      printf("%d", *ptr); 
      picked = *ptr;
      ptr++;
    }
    printf("\n");
  }

	int hitpoint = 1;
	for(int i=0; i<=iS; i++)
	{
		for(int j=0; j<=iT; j++)
		{
			for(int k=0; k<=iU; k++)
			{
				if(hitpoint++ == picked)
				{
					grabCP_x = i;
					grabCP_y = j;
					grabCP_z = k;
				}
			}
		}
	}
	if(grabCP_x == -1 || grabCP_y ==-1 || grabCP_z == -1)
	{
		cout<<"CONTROL POINT IS NOT SELECTED"<<endl;
		grabCP_x = 2;
		grabCP_x = 2;
		grabCP_x = 2;
		isSelectedCP = false;
	}
	else
		isSelectedCP = true;
}
void picking(int button, int state, int x, int y)
{
	GLint hits;
	GLuint selectBuf[BUFSIZE];
	GLint viewport[4];

	if(button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
		return;
	glGetIntegerv(GL_VIEWPORT, viewport);

	glSelectBuffer(BUFSIZE,selectBuf);
	glRenderMode(GL_SELECT);

	glInitNames();
	glPushName(0);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPickMatrix((GLdouble)x, (GLdouble)(viewport[3]-y),5.0,5.0,viewport);
	if(ORTHO_PROJECTION_ID == iProjection_mode)
		glOrtho(-2.5f*xy_aspect,2.5f*xy_aspect,-2.5f,2.5f,0.1f,100.0f);
	if(PERSP_PROJECTION_ID == iProjection_mode)
		gluPerspective(60.0f, xy_aspect, 0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
				glRotatef(obj_x_rotate, 0.0f,1.0f,0.0f);
				glRotatef(obj_y_rotate, 1.0f,0.0f,0.0f);
				glScalef(scale_obj, scale_obj,scale_obj);
				glTranslatef(obj_x_dis, obj_y_dis,0.0f);
				glTranslatef(fObj_position[0], fObj_position[1], -fObj_position[2]);			
				glMultMatrixf(fView_rotation);
	draw(GL_SELECT);
	glPopMatrix();
	glPopMatrix();
	glFlush();

	hits = glRenderMode(GL_RENDER);
	processHits(hits,selectBuf);
}
void moveControlPt(int x, int y)
{
	if(!isSelectedCP)
		return;

	glGetIntegerv(GL_VIEWPORT,viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX,mvmatrix);
	glGetDoublev(GL_PROJECTION_MATRIX,projmatrix);

	realy = viewport[3] - (GLint)y -1;

	gluProject(FFD_pt[grabCP_x][grabCP_y][grabCP_z].x,
		FFD_pt[grabCP_x][grabCP_y][grabCP_z].y,
		FFD_pt[grabCP_x][grabCP_y][grabCP_z].z,
		mvmatrix,projmatrix,viewport,&vx,&vy,&vz);

	gluUnProject((GLdouble)x, (GLdouble)realy, vz, mvmatrix,
		projmatrix,viewport,&cwx,&cwy,&cwz);

	FFD_pt[grabCP_x][grabCP_y][grabCP_z].x = cwx;
	FFD_pt[grabCP_x][grabCP_y][grabCP_z].y = cwy;
	FFD_pt[grabCP_x][grabCP_y][grabCP_z].z = cwz;
}
void draw(GLenum mode)
{
	int ctr = 1;
	for(int i=0; i<=iS; i++)
	{
		for(int j=0; j<=iT; j++)
		{
			for(int k=0; k<=iU; k++)
			{
				glPointSize(10.0);
				if(mode == GL_SELECT)
					glLoadName(ctr++);
				glBegin(GL_POINTS);
					glVertex3d(FFD_pt[i][j][k].x,FFD_pt[i][j][k].y,FFD_pt[i][j][k].z);
				glEnd();
			}
		}
	}
}
int factorial(int n){
    if(n <= 1)
        return 1;
    else 
        return n * factorial(n-1);
}
int i_factorial(int i_n, int i_i)
{
	int temp3, temp1,temp2;
	temp3 = factorial(i_n);
	temp1 = factorial(i_i);
	temp2 = factorial(i_n-i_i);

	return (temp3/(temp2*temp1));
}
void computePijk(int l, int m, int n)
{
	//double s,t,u;
	//int templ,tempm,tempn;
	FFD_pt = new HE_vert**[l+1];
	dWeight = new GLdouble**[l+1];
	for ( int ll = 0; ll<l+1; ll++)
	{
		FFD_pt[ll] = new HE_vert*[m+1];
		dWeight[ll] = new GLdouble*[m+1];
		for ( int mm=0; mm<m+1; mm++)
		{
			FFD_pt[ll][mm] = new HE_vert[n+1];
			dWeight[ll][mm] = new GLdouble[n+1];
		}
	}

	FFD_pt[0][0][0].x = bottom_vertex[0].x; FFD_pt[0][0][0].y = bottom_vertex[0].y; FFD_pt[0][0][0].z = bottom_vertex[0].z;
	FFD_pt[l][0][0].x = top_vertex[2].x; FFD_pt[l][0][0].y = bottom_vertex[0].y; FFD_pt[l][0][0].z = bottom_vertex[0].z;
	FFD_pt[0][m][0].x = bottom_vertex[0].x; FFD_pt[0][m][0].y = top_vertex[2].y; FFD_pt[0][m][0].z = bottom_vertex[0].z;
	FFD_pt[0][0][n].x = bottom_vertex[0].x; FFD_pt[0][0][n].y = bottom_vertex[0].y; FFD_pt[0][0][n].z = top_vertex[2].z;

	vec_S.x = FFD_pt[l][0][0].x - FFD_pt[0][0][0].x, vec_S.y = FFD_pt[l][0][0].y - FFD_pt[0][0][0].y, vec_S.z = FFD_pt[l][0][0].z - FFD_pt[0][0][0].z;
	vec_T.x = FFD_pt[0][m][0].x - FFD_pt[0][0][0].x, vec_T.y = FFD_pt[0][m][0].y - FFD_pt[0][0][0].y, vec_T.z = FFD_pt[0][m][0].z - FFD_pt[0][0][0].z;
	vec_U.x = FFD_pt[0][0][n].x - FFD_pt[0][0][0].x, vec_U.y = FFD_pt[0][0][n].y - FFD_pt[0][0][0].y, vec_U.z = FFD_pt[0][0][n].z - FFD_pt[0][0][0].z;

		/*s = (vertex[i_vert].x-bottom_vertex[0].x)/(top_vertex[2].x-bottom_vertex[0].x);
		t = (vertex[i_vert].y-bottom_vertex[0].x)/(top_vertex[2].y-bottom_vertex[0].y);
		u = (vertex[i_vert].z-bottom_vertex[0].x)/(top_vertex[2].z-bottom_vertex[0].z);*/

	/*	vertex[i_vt].x =0;
		vertex[i_vert].y =0;
		vertex[i_vert].z =0;*/

		for(int i = 0; i <= l; i++)
			for(int j = 0; j <= m; j++)
				for(int k = 0; k<=n; k++)
				{
					FFD_pt[i][j][k].x = FFD_pt[0][0][0].x + (((double)i/l)*vec_S.x);
					FFD_pt[i][j][k].y = FFD_pt[0][0][0].y + (((double)j/m)*vec_T.y);
					FFD_pt[i][j][k].z = FFD_pt[0][0][0].z + (((double)k/n)*vec_U.z);

					//templ = i_factorial(l,i); tempm = i_factorial(m,j); tempn = i_factorial(n,k);
					//dWeight[i][j][k] = (double)templ*pow(s,i)*pow((1-s),(l-i))+(double)tempm*pow(t,j)*pow((1-t),(m-j))*(double)tempn*pow(u,k)*pow((1-u),(n-k));

					/*vertex[i_ve].x += FFD_pt[i][j][k].x * dWeight[i][j][k];
					vertex[i_vert].y += FFD_pt[i][j][k].y * dWeight[i][j][k];
					vertex[i_vert].z += FFD_pt[i][j][k].z * dWeight[i][j][k];*/
				}
	
	old_iS = l, old_iT = m, old_iU = n;
}
void computeVert()
{
	double s,t,u;//HE_vert ffd3, ffd2, ffd1;
	//double bpS = 0, bpT = 0, bpU = 0;
	int templ,tempm,tempn;
	for(int i_vert = 0; i_vert<i_vertex_num; i_vert++)
	{
		s = (v0[i_vert].x-bottom_vertex[0].x)/(top_vertex[2].x-bottom_vertex[0].x);
		t = (v0[i_vert].y-bottom_vertex[0].y)/(top_vertex[2].y-bottom_vertex[0].y);
		u = (v0[i_vert].z-bottom_vertex[0].z)/(top_vertex[2].z-bottom_vertex[0].z);

		vertex[i_vert].x =0;
		vertex[i_vert].y =0;
		vertex[i_vert].z =0;
		
		for(int i = 0; i <= iS; i++)
			for(int j = 0; j <= iT; j++)
				for(int k = 0; k<=iU; k++)
				{
					templ = i_factorial(iS,i); tempm = i_factorial(iT,j); tempn = i_factorial(iU,k);
					dWeight[i][j][k] = (double)templ*pow(s,i)*pow((1-s),(iS-i))*(double)tempm*pow(t,j)*pow((1-t),(iT-j))*(double)tempn*pow(u,k)*pow((1-u),(iU-k));

					vertex[i_vert].x += FFD_pt[i][j][k].x * dWeight[i][j][k];
					vertex[i_vert].y += FFD_pt[i][j][k].y * dWeight[i][j][k];
					vertex[i_vert].z += FFD_pt[i][j][k].z * dWeight[i][j][k];
				}
	}
}

//Functions for OpenGL GLUI operations
void render_mesh()
{
	int i;
	/*Points cloud mode*/
	if(iDisplay_mode == DISPLAY_POINTS_ID)
	{
		glDisable(GL_LIGHTING);
		glPointSize(1.0);
		for(i=0; i<i_vertex_num; i++)
		{	
			glBegin(GL_POINTS);
				glVertex3f(vertex[i].x,vertex[i].y,vertex[i].z);
			glEnd();
		}
		glEnable(GL_LIGHTING);
	}
	else if(iDisplay_mode == DISPLAY_WIRE_ID)
	{
		glDisable(GL_LIGHTING);
		for(i=0; i<i_face_num; i++)
		{
			glBegin(GL_LINE_LOOP);
				glVertex3f(vertex[face[i].v1-1].x, vertex[face[i].v1-1].y, vertex[face[i].v1-1].z);
				glVertex3f(vertex[face[i].v2-1].x, vertex[face[i].v2-1].y, vertex[face[i].v2-1].z);
				glVertex3f(vertex[face[i].v3-1].x, vertex[face[i].v3-1].y, vertex[face[i].v3-1].z);
			glEnd();
		}
		glEnable(GL_LIGHTING);
	}
	else if(iDisplay_mode == DISPLAY_FLAT_ID)
	{
		glColor3f(1.0f,1.0f,1.0f);
		for(i=0; i<i_face_num; i++)
		{
			glBegin(GL_TRIANGLES);
				glNormal3f(vertex[face[i].v1-1].normal[0], vertex[face[i].v1-1].normal[1], vertex[face[i].v1-1].normal[2]);
				glVertex3f(vertex[face[i].v1-1].x, vertex[face[i].v1-1].y, vertex[face[i].v1-1].z);
				glNormal3f(vertex[face[i].v2-1].normal[0], vertex[face[i].v2-1].normal[1], vertex[face[i].v2-1].normal[2]);
				glVertex3f(vertex[face[i].v2-1].x, vertex[face[i].v2-1].y, vertex[face[i].v2-1].z);
				glNormal3f(vertex[face[i].v3-1].normal[0], vertex[face[i].v3-1].normal[1], vertex[face[i].v3-1].normal[2]);
				glVertex3f(vertex[face[i].v3-1].x, vertex[face[i].v3-1].y, vertex[face[i].v3-1].z);
			glEnd();
		}
		glColor3f(glObj_r, glObj_g, glObj_b);
	}
	else if(iDisplay_mode == DISPLAY_SMOOTH_ID)
	{
		glColor3f(1.0f,1.0f,1.0f);
		for(i=0; i<i_face_num; i++)
		{
			glBegin(GL_TRIANGLES);
				glNormal3f(vertex[face[i].v1-1].normal[0], vertex[face[i].v1-1].normal[1], vertex[face[i].v1-1].normal[2]);
				glVertex3f(vertex[face[i].v1-1].x, vertex[face[i].v1-1].y, vertex[face[i].v1-1].z);
				glNormal3f(vertex[face[i].v2-1].normal[0], vertex[face[i].v2-1].normal[1], vertex[face[i].v2-1].normal[2]);
				glVertex3f(vertex[face[i].v2-1].x, vertex[face[i].v2-1].y, vertex[face[i].v2-1].z);
				glNormal3f(vertex[face[i].v3-1].normal[0], vertex[face[i].v3-1].normal[1], vertex[face[i].v3-1].normal[2]);
				glVertex3f(vertex[face[i].v3-1].x, vertex[face[i].v3-1].y, vertex[face[i].v3-1].z);
			glEnd();
		}
		glColor3f(glObj_r, glObj_g, glObj_b);
	}
	else
		{
			MessageBox(NULL,"Unexpective error occur when rendering the object! Please try again!","Error", MB_OK);
			exit(0);
		}
}
static void GL_init(void)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

	glEnable(GL_DEPTH_TEST);
	//glDepthRange(0.0,1.0);
}
void display(void)
{
	//Set background color
	GLfloat j;
	glClearColor(glB_r, glB_g, glB_b,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Choose the shading mode
	if(DISPLAY_FLAT_ID == iDisplay_mode)
		glShadeModel(GL_FLAT);
	if( DISPLAY_SMOOTH_ID == iDisplay_mode)
		glShadeModel(GL_SMOOTH);
	
	//Choose the projection mode 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
		if(ORTHO_PROJECTION_ID == iProjection_mode)
			glOrtho(-2.5f*xy_aspect,2.5f*xy_aspect,-2.5f,2.5f,0.1f,100.0f);
		if(PERSP_PROJECTION_ID == iProjection_mode)
			gluPerspective(60.0f, xy_aspect, 0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//Light rotation operation
		glPushMatrix();
			glMultMatrixf(light0_rotation);
			glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
		glPopMatrix();
		glPushMatrix();
			glMultMatrixf(light1_rotation);
			glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
		glPopMatrix();

	//glPushMatrix();
	//Transform the scene
		gluLookAt(0.0f,0.0f,5.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);
		glTranslatef(x_dis,y_dis,0.0f);
		glRotatef(x_rotate,0.0f,1.0f,0.0f);
		glRotatef(y_rotate,1.0f,0.0f,0.0f);
		glScalef(scale_scene,scale_scene,scale_scene);
		

		glColor3f(glObj_r,glObj_g,glObj_b);

	//Draw the 3 axes: cylinder + pyramid
	if(1 == iDraw_axes)
	{
		glDisable(GL_LIGHTING);
		//z
		glColor3f(1.0f,0.0f,0.0f);
		quadratic = gluNewQuadric();
		gluQuadricNormals(quadratic,GLU_SMOOTH);
		gluQuadricTexture(quadratic,GL_FALSE);
		gluCylinder(quadratic,0.02f,0.02f,z_axis,32,32);
		glPushMatrix();
			glTranslatef(0.0f,0.0f,z_axis);
			gluCylinder(quadratic,0.04f,0.0f,0.1f,32,32);
		glPopMatrix();
		//x
		glColor3f(0.0f,1.0f,0.0f);
		glPushMatrix();
			glRotatef(90.0f,0.0f,1.0f,0.0f);
			quadratic = gluNewQuadric();
			gluQuadricNormals(quadratic,GLU_SMOOTH);
			gluQuadricTexture(quadratic, GL_FALSE);
			gluCylinder(quadratic,0.02f,0.02f,x_axis,32,32);
			glPushMatrix();
				glTranslatef(0.0f,0.0f,x_axis);
				gluCylinder(quadratic,0.04f,0.0f,0.1f,32,32);
			glPopMatrix();
		glPopMatrix();
		//y
		glColor3f(0.0f,0.0f,1.0f);
		glPushMatrix();
			glRotatef(-90.0f,1.0f,0.0f,0.0f);
			quadratic = gluNewQuadric();
			gluQuadricNormals(quadratic,GLU_SMOOTH);
			gluQuadricTexture(quadratic, GL_FALSE);
			gluCylinder(quadratic,0.02f,0.02f,y_axis,32,32);
			glPushMatrix();
				glTranslatef(0.0f,0.0f,y_axis);
				gluCylinder(quadratic,0.04f,0.0f,0.1f,32,32);
			glPopMatrix();
		glPopMatrix();
		glColor3f(glObj_r, glObj_g, glObj_b);
		glEnable(GL_LIGHTING);
	}
	//Draw the grid
	if( 1 == iDraw_grid)
	{
		glDisable(GL_LIGHTING);
		glLineWidth(2.0f);
		glColor3f(1.0f,1.0f,1.0f);
		glBegin(GL_LINES);
			for(j=-10.0f;j<0.0f;j+=0.5f)
			{
				glVertex3f(j,0.0f,-10.0f);
				glVertex3f(j,0.0f,10.0f);
			}
			for(j=0.5f; j<=10.0f; j+=0.5)
			{
				glVertex3f(j,0.0f,-10.0f);
				glVertex3f(j,0.0f,10.0f);
			}
			for(j=-10.0f;j<0.0f;j+=0.5f)
			{
				glVertex3f(-10.0f,0.0f,j);
				glVertex3f(10.0f,0.0f,j);
			}
			for(j=0.5f;j<=10.0f; j+=0.5)
			{
				glVertex3f(-10.0f,0.0f,j);
				glVertex3f(10.0f,0.0f,j);
			}
		glEnd();
		glColor3f(0.0f,0.0f,0.0f);
		glBegin(GL_LINES);
			glVertex3f(0.0f,0.0f,-10.0f);
			glVertex3f(0.0f,0.0f,10.0f);
			glVertex3f(-10.0f,0.0f,0.0f);
			glVertex3f(10.0f,0.0f,0.0f);
		glEnd();
		glColor3f(glObj_r, glObj_g, glObj_b);
		glLineWidth(1.0f);
		glEnable(GL_LIGHTING);
	}
	
	//Import the file and render it 
	if(1 == load)
	{	
		//If reload, initialize the scene to the original one
		if(1 == reload)
		{
			for(int n=0;n<i_edge_num;n++)
				edge[n].pair = NULL;

			fView_rotation [0] = fView_rotation [5] = fView_rotation [10] =fView_rotation [15] = 1.0;
			fView_rotation [1] = fView_rotation [2] = fView_rotation [3] = fView_rotation [4]=0.0;
			fView_rotation [6] = fView_rotation [7] = fView_rotation [8] = fView_rotation [9]=0.0;
			fView_rotation [11] = fView_rotation [12] = fView_rotation [13] = fView_rotation [14]=0.0;
			fObj_position [0] = fObj_position [1] = fObj_position [2] = 0.0; 
			x_dis = 0.0f;   x_rotate = -30.0f;
			y_dis = -1.0f;  y_rotate = 0.0f;
			scale_scene = 0.5f;
			obj_x_dis = obj_y_dis = 0.0f;
			obj_x_rotate = obj_y_rotate = 0.0f;
			scale_obj = 1.0f;
		}
		//Bounding box
		if(1 == iDraw_boundingbox)
		{
			glDisable(GL_LIGHTING);
			glColor3f(1.0f,0.5f,0.5f);
			glLineWidth(2.0f);
			glPushMatrix();
				glRotatef(obj_x_rotate, 0.0f,1.0f,0.0f);
				glRotatef(obj_y_rotate, 1.0f,0.0f,0.0f);
				//glScalef(1.5/temp,1.5/temp,1.5/temp);
				glScalef(scale_obj, scale_obj,scale_obj);
				glTranslatef(obj_x_dis, obj_y_dis,0.0f);
				glTranslatef(fObj_position[0], fObj_position[1], -fObj_position[2]);			
				glMultMatrixf(fView_rotation);
				glPointSize(10.0);

				draw(GL_RENDER);/*glBegin(GL_POINTS);
				/*for(int s =;s<=iS;s++)
					for(int t=0;t<=iT;t++)
						for(int u=0;u<=iU;u++)
							glVertex3d(FFD_pt[s][t][u].x,FFD_pt[s][t][u].y,FFD_pt[s][t][u].z);
				glEnd();*/

				glLineWidth(2.0);
				glBegin(GL_LINES);
				//xy plane
				for(int s =0; s<=iS; s++)
					for(int t=0; t<=iT; t++)
						for(int u=0; u<iU; u++)
						{
								glVertex3d(FFD_pt[s][t][u].x,FFD_pt[s][t][u].y,FFD_pt[s][t][u].z);
								glVertex3d(FFD_pt[s][t][u+1].x,FFD_pt[s][t][u+1].y,FFD_pt[s][t][u+1].z);
						}
					for(int s =0; s<=iS; s++)
					for(int t=0; t<iT; t++)
						for(int u=0; u<=iU; u++)
						{
								glVertex3d(FFD_pt[s][t][u].x,FFD_pt[s][t][u].y,FFD_pt[s][t][u].z);
								glVertex3d(FFD_pt[s][t+1][u].x,FFD_pt[s][t+1][u].y,FFD_pt[s][t+1][u].z);
						}
					//yz plane
					for(int s =0; s<iS; s++)
						for(int t=0; t<=iT; t++)
							for(int u=0; u<=iU; u++)
						{
								glVertex3d(FFD_pt[s][t][u].x,FFD_pt[s][t][u].y,FFD_pt[s][t][u].z);
								glVertex3d(FFD_pt[s+1][t][u].x,FFD_pt[s+1][t][u].y,FFD_pt[s+1][t][u].z);
						}
				glEnd();
			glPopMatrix();
			glColor3f(glObj_r,glObj_g,glObj_b);
			glEnable(GL_LIGHTING);
		}
		if(1 == iDraw_edges)
		{
			glDisable(GL_LIGHTING);
			glColor3f(0.0f,0.0f,0.0f);
			glLineWidth(2.0f);
			glPushMatrix();
				glRotatef(obj_x_rotate, 0.0f,1.0f,0.0f);
				glRotatef(obj_y_rotate, 1.0f,0.0f,0.0f);
				//glScalef(1.5/temp,1.5/temp,1.5/temp);
				glScalef(scale_obj, scale_obj,scale_obj);
				//glTranslatef(-top_vertex[0].x,-bottom_vertex[0].y,-top_vertex[0].z);
				glTranslatef(obj_x_dis, obj_y_dis,0.0f);
				glTranslatef(fObj_position[0], fObj_position[1], -fObj_position[2]);			
				glMultMatrixf(fView_rotation);
					for(int i=0;i<i_face_num;i++)
					{
						glBegin(GL_LINE_LOOP);
							glVertex3f(vertex[face[i].v1-1].x, vertex[face[i].v1-1].y, vertex[face[i].v1-1].z);
							glVertex3f(vertex[face[i].v2-1].x, vertex[face[i].v2-1].y, vertex[face[i].v2-1].z);
							glVertex3f(vertex[face[i].v3-1].x, vertex[face[i].v3-1].y, vertex[face[i].v3-1].z);
						glEnd();				
					}
			glPopMatrix();
			glColor3f(glObj_r,glObj_g,glObj_b);
			glEnable(GL_LIGHTING);
		}
		//Render the object.
		glPushMatrix();
			glRotatef(obj_x_rotate, 0.0f,1.0f,0.0f);
			glRotatef(obj_y_rotate, 1.0f,0.0f,0.0f);
			//glScalef(1.5/temp,1.5/temp,1.5/temp);
			glScalef(scale_obj, scale_obj,scale_obj);
			//glTranslatef(-top_vertex[0].x,-bottom_vertex[0].y,-top_vertex[0].z);
			glTranslatef(obj_x_dis, obj_y_dis,0.0f);
			glTranslatef(fObj_position[0], fObj_position[1], -fObj_position[2]);			
			glMultMatrixf(fView_rotation);
			/*compute_vert();*/
			render_mesh();
			/*computeVert();*/
		glPopMatrix();
	}
	//glPopMatrix();
	reload = 0;
	glutPostRedisplay();
	glEnable(GL_LIGHTING);
	glutSwapBuffers();
}              
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	xy_aspect = (GLfloat)w/(GLfloat)h;
	glutPostRedisplay();
}
void control(int control)
{
	//open file dialog
	OPENFILENAME _file_name;
	//color dialog
	CHOOSECOLOR _choose_color;
	static char scFile[256];
	static char scFileTitle[256];
	COLORREF CustColor[16];
	HWND hWND = NULL;

	//Initilize the open file dialog operation
	memset(&_file_name,0,sizeof(_file_name));
	_file_name.lStructSize = sizeof(_file_name);
	_file_name.hwndOwner = hWND;
	_file_name.lpstrFilter ="3D Model object(*m)\0*.m;\0\0";
	_file_name.nFilterIndex =1;
	_file_name.lpstrFile = scFile;
	_file_name.nMaxFile = sizeof(scFile);
	_file_name.lpstrFileTitle = scFileTitle;
	_file_name.nMaxFileTitle = sizeof(scFileTitle);
	_file_name.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;

	//Initialize color dialog operation
	_choose_color.lStructSize = sizeof(_choose_color);
	_choose_color.hwndOwner = hWND;
	_choose_color.hInstance = NULL;
	_choose_color.lpCustColors = CustColor;
	_choose_color.Flags = CC_FULLOPEN;

	/* From here deal with the call back issues*/
	//If press "Import Mesh" button
	if (IMPORT_ID == control)
	{
		if(GetOpenFileName(&_file_name))
		{
			mesh_import(scFileTitle);
			
			boundingbox();
			settoorigin(vertex,bottom_vertex);
			boundingbox();
			computePijk(iS,iT,iU);
			compute_vert_norm(vertex);
			for(int i_vert = 0; i_vert<i_vertex_num; i_vert++)
			{
				v0[i_vert] = vertex[i_vert];
			}
			reload = load =1;	
		}	
	}
	if(BACKGROUND_COLOR_ID == control)
		if(ChooseColor(&_choose_color))
		{
			//Convert the r,g,b color that choosed
			glB_r = (GLfloat) GetRValue(_choose_color.rgbResult) /(GLfloat) 255;
			glB_g = (GLfloat) GetGValue(_choose_color.rgbResult) /(GLfloat) 255;
			glB_b = (GLfloat) GetBValue(_choose_color.rgbResult) /(GLfloat) 255;
		}
	if(OBJECT_COLOR_ID == control)
		if(ChooseColor(&_choose_color))
		{
			//Convert the r,g,b color 
			LightDiffuse[0] = (GLfloat) GetRValue(_choose_color.rgbResult) /(GLfloat)255;
			LightDiffuse[1] = (GLfloat) GetGValue(_choose_color.rgbResult) /(GLfloat)255;
			LightDiffuse[2] = (GLfloat) GetBValue(_choose_color.rgbResult) /(GLfloat)255;
			glObj_r = LightDiffuse[0];
			glObj_g = LightDiffuse[1];
			glObj_b = LightDiffuse[2];
		}
	if(LIGHT0_ENABLE_ID == control)
	{
			if(iLight0_enabled)
		{
			glEnable(GL_LIGHT0);
			light0_spinner->enable();
		}
		else
		{
			glDisable(GL_LIGHT0);
			light0_spinner->disable();
		}
	}
	if(LIGHT1_ENABLE_ID == control)
	{
		if(iLight1_enabled)
		{
			glEnable(GL_LIGHT1);
			light1_spinner->enable();
		}
		else
		{
			glDisable(GL_LIGHT1);
			light1_spinner->disable();
		}
	}
	if(LIGHT0_INTENSITY_ID == control)
	{
		float fv_diffuse[] = {
			light0_diffuse[0], light0_diffuse[1],
			light0_diffuse[2], light0_diffuse[3]};
		fv_diffuse[0] *=fLight0_intensity;
		fv_diffuse[1] *=fLight0_intensity;
		fv_diffuse[2] *=fLight0_intensity;
		glLightfv(GL_LIGHT0, GL_DIFFUSE, fv_diffuse);
	}
	if(LIGHT1_INTENSITY_ID == control)
	{
		float fv_diffuse[] = {
			light1_diffuse[0], light1_diffuse[1],
			light1_diffuse[2], light1_diffuse[3]};
		fv_diffuse[0] *=fLight1_intensity;
		fv_diffuse[1] *=fLight1_intensity;
		fv_diffuse[2] *=fLight1_intensity;
		glLightfv(GL_LIGHT1, GL_DIFFUSE, fv_diffuse);
	}
	if(DISPLAY_ID == control)
	{
		//Get the value of display panel
		iDisplay_mode = 200 + radio_group_display->get_int_val();
	}
	if(PROJECTION_ID == control)
	{
		iProjection_mode = 300 + radio_group_projection->get_int_val();
	}
	if(FFD_ID == control)
	{
		for(int i=0; i<= old_iS; ++i)   
		{   
			for(int j=0; j <=old_iT;++j)   
			{  
				delete []FFD_pt[i][j]; 
			}  
		}   
		delete []FFD_pt; 

		computePijk(iS,iT,iU);	
	}
	if(FFD_RESET_ID == control)
	{
		for(int i=0; i<i_vertex_num; i++)
		{
			vertex[i].x=v0[i].x;
			vertex[i].y=v0[i].y;
			vertex[i].z=v0[i].z;
		}
	}
}
void mouse(int button, int state, int x, int y)
{
	if( GLUT_DOWN == state)
	{
		if(isPicking == 0)
		{
			iMouse_x = x; iMouse_y = y;
			if(GLUT_LEFT_BUTTON == button)
				iTransform_mode = TRANSFORM_ROTATION_ID;
			if(GLUT_MIDDLE_BUTTON == button)
				iTransform_mode = TRANSFORM_TRANSLATION_ID;
			if(GLUT_RIGHT_BUTTON == button)
				iTransform_mode = TRANSFORM_SCALING_ID;
		}
		if(isPicking ==1 && GLUT_LEFT_BUTTON == button)
		{
			{
				iTransform_mode = TRANSFORM_ROTATION_ID;
				picking(button,state,x,y);
				//moveControlPt(x,y);
				//computeVert();
			}
		}
	}
	if(GLUT_UP == state)
	{
		glRenderMode(GL_RENDER);
		iTransform_mode = TRANSFORM_NONE_ID;
	}
	//glutPostRedisplay();
}
void mouse_motion(int x, int y)
{
	if(0 == iObject)
	{
		
		if(TRANSFORM_ROTATION_ID == iTransform_mode)
		{
			x_rotate += (GLfloat)(x-iMouse_x)/5.0;
			if(x_rotate >180.0)
				x_rotate -=360.0;
			if(x_rotate <-180.0)
				x_rotate +=360.0;
			iMouse_x = x;

			y_rotate += (GLfloat)(y-iMouse_y)/5.0;
			if(y_rotate >180.0)
				y_rotate -=360.0;
			if(y_rotate <-180.0)
				y_rotate +=360.0;
			iMouse_y = y;
		}
		if(TRANSFORM_TRANSLATION_ID == iTransform_mode)
		{
			x_dis += (GLfloat)(x-iMouse_x)/30.0f;
			iMouse_x = x;
			y_dis += (GLfloat)(y-iMouse_y)/30.0f;
			iMouse_y = y;
		}
		if(TRANSFORM_SCALING_ID == iTransform_mode)
		{
			Ini_size = scale_scene;
			scale_scene *= (1+(y-iMouse_y)/60.0);
			if(scale_scene <0)
				scale_scene = Ini_size;
			iMouse_y = y;
		}
	}
	if(1 == iObject)
	{
		if(isPicking == 0)
		{
		if(TRANSFORM_ROTATION_ID == iTransform_mode)
		{
			
			obj_x_rotate += (GLfloat)(x-iMouse_x)/5.0;
			if(obj_x_rotate >180.0)
				obj_x_rotate -=360.0;
			if(obj_x_rotate <-180.0)
				obj_x_rotate +=360.0;
			iMouse_x = x;

			obj_y_rotate += (GLfloat)(y-iMouse_y)/5.0;
			if(obj_y_rotate >180.0)
				obj_y_rotate -=360.0;
			if(obj_y_rotate <-180.0)
				obj_y_rotate +=360.0;
			iMouse_y = y;
			
			
		}
		if(TRANSFORM_TRANSLATION_ID == iTransform_mode)
		{
			obj_x_dis += (GLfloat)(x-iMouse_x)/30.0f;
			iMouse_x = x;
			obj_y_dis += (GLfloat)(y-iMouse_y)/30.0f;
			iMouse_y = y;
		}
		if(TRANSFORM_SCALING_ID == iTransform_mode)
		{
			Ini_size = scale_obj;
			scale_obj *= (1+(y-iMouse_y)/60.0);
			if(scale_obj <0)
				scale_obj = Ini_size;
			iMouse_y = y;
		}
	}
		if(isPicking ==1 && iTransform_mode == TRANSFORM_ROTATION_ID)
		{
			moveControlPt(x,y);
			
			glMatrixMode(GL_MODELVIEW);
			glFlush();
			computeVert();

			/*(void) glRenderMode(GL_RENDER);
			for(int i=0; i<i_vertex_num;i++)
				vertex[i] = v0[i];*/

		}
	}
		glutPostRedisplay();
}
	


