/******************************************************************
* PROGRAMMING EXERCISE 3 - Rotating Mobile
* authors: 	Daniel Eberharter
*	  	Stefan Haselwanter
* date: 	10.06.2014   
* version:	1.0
* desc.:	draws and animates a simple rotating mobile with
		basic mirroring, texturing and billboarding

		the code may contain fragments of the programming
		example no. 2 from Prof. Matthias Harders.
 * controls: 	w,a,s,d ... move camera
		0 	... toggle shading
		f	... toggle ambient lighting LIGHT0
		g	... toggle diffuse lighting LIGHT0
		h	... toggle specular lighting LIGHT0
		v	... increase hue of LIGHT0
		b	... decrease hue of LIGHT0
		n	... increase val of LIGHT0
		m	... decrease val of LIGHT0
*******************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "ObjectGl.h"
#include "OBJParserMobile.h"
#include "LoadShader.h"  
#include "Matrix.h"  
#include "Wall.h"
#include "Billboard.h"
#include "Util.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define BACKGROUND_COLOR_R	 0.3
#define BACKGROUND_COLOR_G	 0.3	
#define BACKGROUND_COLOR_B	 0.3
#define FOVY			50.0
#define NEAR_PLANE	 	 0.5
#define FAR_PLANE		70.0
#define ASPECT			 1
#define CAMERA_DIST		30
#define CAMERA_ROTATE_ANGLE	 1

#define BTN_UP			'w'
#define BTN_DOWN		's'
#define BTN_LEFT		'a'
#define BTN_RIGHT		'd'
#define BTN_TGL_SHADING		'0'
#define BTN_TGL_AMBIENT		'f'
#define BTN_TGL_DIFFUSE		'g'
#define BTN_TGL_SPECULAR	'h'
#define BTN_HUE_UP		'v'
#define BTN_HUE_DOWN		'b'
#define BTN_VALUE_UP		'm'
#define BTN_VALUE_DOWN		'n'


//Shaders
#define GOURAUD_VS		"shaders/vertexshader_gouraud.vs"
#define GOURAUD_FS		"shaders/fragmentshader_gouraud.fs"
#define GOURAUD_SHADER_CONST	0
#define PHONG_VS		"shaders/vertexshader_phong.vs"
#define PHONG_FS		"shaders/fragmentshader_phong.fs"
#define PHONG_SHADER_CONST	1
#define INIT_SHADER_CONST	GOURAUD_SHADER_CONST

#define NUM_LIGHT		2
#define NUM_WALLS		1

//Lighting
#define LIGHT0_POSITION		{ 10.0, 10.0, 10.0,  1.0 }
#define LIGHT0_INTENSITY	{  1.0,  1.0,  1.0,  1.0 }
#define LIGHT0_SPECULAR		{  1.0,  1.0,  1.0,  1.0 }
#define LIGHT0_AMBIENT		{  1.0,  0.0,  0.0,  1.0 }
#define LIGHT0_DIFFUSE		{  1.0,  1.0,  1.0,  1.0 }

#define LIGHT1_POSITION		{-10.0, 10.0, 10.0,  1.0 }
#define LIGHT1_INTENSITY	{  1.0,  1.0,  1.0,  1.0 }
#define LIGHT1_SPECULAR		{  1.0,  1.0,  1.0,  1.0 }
#define LIGHT1_AMBIENT		{  1.0,  1.0,  1.0,  1.0 }
#define LIGHT1_DIFFUSE		{  1.0,  1.0,  1.0,  1.0 }


/******************************************************************
* globals
*******************************************************************/
GLuint shader_program[2];
static const char* VertexShaderString;
static const char* FragmentShaderString;		
short shader_idx = INIT_SHADER_CONST;

float proj_matrix[16]; 	
float view_matrix[16]; 	

node_object *root;
object_gl *walls[NUM_WALLS];
object_gl *billboard;
lightsource light[NUM_LIGHT];
int last_billboard_animation = 0;

float billboard_step_x = STEP_X;
float billboard_step_y = STEP_Y;


/******************************************************************
* draw mobile
*******************************************************************/
void draw_mobile(node_object node){
	draw_single(&(node.obj), proj_matrix, view_matrix, shader_program[shader_idx], light, NUM_LIGHT);

	if(node.child_l != NULL)
		draw_mobile(*(node.child_l));

	if(node.child_r != NULL)
		draw_mobile(*(node.child_r));
}

void draw_mobile_mirror(node_object node, double *scale, double *translation){
	draw_single_mirror(&(node.obj), proj_matrix, view_matrix, shader_program[shader_idx], light, NUM_LIGHT, scale, translation);

	if(node.child_l != NULL)
		draw_mobile_mirror(*(node.child_l), scale, translation);

	if(node.child_r != NULL)
		draw_mobile_mirror(*(node.child_r), scale, translation);
}

/******************************************************************
* display

*******************************************************************/
void display(){
	//clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	//fill stencil buffer
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 0xFFFFFFFF);

	draw_single(walls[0], proj_matrix, view_matrix, shader_program[shader_idx], light, NUM_LIGHT);

 	// reenabled color/depth 
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glEnable(GL_DEPTH_TEST);

  	// draw reflection
	glStencilFunc(GL_EQUAL, 1, 0xffffffff); 
  	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	double scale[] = { 1.0, -1.0, 1.0 };
	double translation[] = { 0.0, -20.0, 0.0 };
	draw_mobile_mirror(*root, scale, translation);
	draw_single_mirror(billboard, proj_matrix, view_matrix, shader_program[shader_idx], light, NUM_LIGHT, scale, translation);

	//draw actual objects
	glDisable(GL_STENCIL_TEST);

	draw_mobile(*root);
	draw_single(billboard, proj_matrix, view_matrix, shader_program[shader_idx], light, NUM_LIGHT);

	glEnable (GL_BLEND); 
	glBlendFunc (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	draw_single(walls[0], proj_matrix, view_matrix, shader_program[shader_idx], light, NUM_LIGHT);
  
	glutSwapBuffers();
}

/******************************************************************
* orbit_subtree
*******************************************************************/
void orbit_subtree(node_object *orbit, node_object *center, float degree) {
	orbit_object(&(orbit->obj), &(center->obj), degree);

	if(orbit->child_l != NULL)
		orbit_subtree(orbit->child_l, center, degree);

	if(orbit->child_r != NULL)
		orbit_subtree(orbit->child_r, center, degree);
}

/******************************************************************
* rotate mobile
*******************************************************************/
void rotate_mobile(node_object *node) {
	float now = glutGet(GLUT_ELAPSED_TIME);
	float time_delta = now - node->obj.rotation_dur;
	node->obj.rotation_dur = now;	

	if(node->child_l != NULL){
		rotate_mobile(node->child_l);
		orbit_subtree(node->child_l, node, time_delta * node->obj.rotation_spd * node->obj.rotation_dir);
	}
	if(node->child_r != NULL){
		rotate_mobile(node->child_r);
		orbit_subtree(node->child_r, node, time_delta * node->obj.rotation_spd * node->obj.rotation_dir);
	}

	rotate_object(&(node->obj), time_delta * node->obj.rotation_spd * node->obj.rotation_dir);
}

/******************************************************************
* animate billboard
* Iterate through UV coordinates from top-left to down-right.
*******************************************************************/
void animate_billboard(){
	int now = glutGet(GLUT_ELAPSED_TIME);
	if(now - last_billboard_animation < ANIMATE_TIME) {
		return;
	}
	last_billboard_animation = now;

	if (	billboard->color_buffer_data[0] + billboard_step_x > 1.0 - UV_OFFSET ||
		billboard->color_buffer_data[0] + billboard_step_x < 0.0 - UV_OFFSET){
		// Decrease V of each vertex and reset U.

		for (int j = 0; j < billboard->num_vertx; j++)
				billboard->color_buffer_data[2*j + 1] -= billboard_step_y;

		if(billboard_step_y > 0) {
			
			billboard->color_buffer_data[0] = 0.00 + UV_OFFSET;
			billboard->color_buffer_data[2] = 0.25 + UV_OFFSET;
			billboard->color_buffer_data[4] = 0.00 + UV_OFFSET;
			billboard->color_buffer_data[6] = 0.25 + UV_OFFSET;
			
		} else {
			billboard->color_buffer_data[0] = 0.75 + UV_OFFSET;
			billboard->color_buffer_data[2] = 1.00 + UV_OFFSET;
			billboard->color_buffer_data[4] = 0.75 + UV_OFFSET;
			billboard->color_buffer_data[6] = 1.00 + UV_OFFSET;
		}
	}

	// Reach down-right or  corner.
	if (	(billboard->color_buffer_data[2] + billboard_step_x > 1.0 + UV_OFFSET && billboard->color_buffer_data[3] <= 0.00) ||
		(billboard->color_buffer_data[0] + billboard_step_x < 0.0 + UV_OFFSET && billboard->color_buffer_data[1] >= 0.66) ){
		// Reset U and V to start coordinate

		printf("blub\n");
		billboard_step_x *= -1.0;
		billboard_step_y *= -1.0;		
	}

	for (int i = 0; i < billboard->num_vertx; i++){
		billboard->color_buffer_data[2*i] += billboard_step_x;

		printf("UV %d: %f %f\n", i, billboard->vertx_texture[i].UV[0], billboard->vertx_texture[i].UV[1]);
	}

	printf("\n\n");
}

/******************************************************************
* on idle
*******************************************************************/
void on_idle(){
	rotate_mobile(root);
	animate_billboard();
	glutPostRedisplay();
}

/******************************************************************
* add shader
*******************************************************************/
void add_shader(GLuint shader_program, const char* ShaderCode, GLenum ShaderType){
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(0);
	}

	/* Associate shader source code string with shader object */
	glShaderSource(ShaderObj, 1, &ShaderCode, NULL);

	GLint success = 0;
	GLchar InfoLog[1024];

	/* Compile shader source code */
	glCompileShader(ShaderObj);
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		exit(1);
	}

	/* Associate shader with shader program */
	glAttachShader(shader_program, ShaderObj);
}


/******************************************************************
* create shader program 
*******************************************************************/
GLint create_shader_program(char *vertx_shader, char *fragm_shader){
	/* Allocate shader object */
	GLint shader_program = glCreateProgram();

	if (shader_program == 0) {
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}

	/* Load shader code from file */
	VertexShaderString   = LoadShader(vertx_shader);
	FragmentShaderString = LoadShader(fragm_shader);

	/* Separately add vertex and fragment shader to program */
	add_shader(shader_program, VertexShaderString, GL_VERTEX_SHADER);
	add_shader(shader_program, FragmentShaderString, GL_FRAGMENT_SHADER);

	/* Link shader code into executable shader program */
	glLinkProgram(shader_program);

	/* Check results of linking step */
	GLint Success = 0;
	GLchar ErrorLog[1024];
	glGetProgramiv(shader_program, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		glGetProgramInfoLog(shader_program, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	/* Check if shader program can be executed */ 
	glValidateProgram(shader_program);
	glGetProgramiv(shader_program, GL_VALIDATE_STATUS, &Success);

	if (!Success) {
		glGetProgramInfoLog(shader_program, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	return shader_program;
}

/******************************************************************
* init shaders
*******************************************************************/
void init_shaders(void){
	shader_program[GOURAUD_SHADER_CONST] = create_shader_program(GOURAUD_VS, GOURAUD_FS);
	shader_program[PHONG_SHADER_CONST]   = create_shader_program(PHONG_VS, PHONG_FS);

	glUseProgram(shader_program[shader_idx]);
}

/******************************************************************
* init mobile
*******************************************************************/
void init_object_mobile(node_object *node) {
	init_object(&(node->obj));

	if(node->child_l != NULL)
		init_object_mobile(node->child_l);
	if(node->child_r != NULL)
		init_object_mobile(node->child_r);
}

/******************************************************************
* init objects
*******************************************************************/
void init_objects() {
	root = parse_mobile("data/mobile.obj");

	if(root == NULL) {
		printf("error parsing file...\n");
		exit(EXIT_FAILURE);
	}

	// Walls
	walls[0] = create_wallXZ(-20.0,-20.0,-20.0, 20.0,-20.0, 20.0, "data/mirror.bmp");
	walls[0]->alpha = 0.1;

	// Billboard
	billboard = create_board(-2.0, 6.0, 0.0, 2.0, 12.0, 0.0);

	if(walls[0] == NULL){
		printf("error creating grid...\n");
		exit(EXIT_FAILURE);
	}

	if(billboard == NULL){
		printf("error creating billboard...\n");
		exit(EXIT_FAILURE);
	}
}

/******************************************************************
* init lights
*******************************************************************/
void init_lights() {
	//init first light
	GLfloat light0_intensity[]= LIGHT0_INTENSITY;
	memcpy( light[0].intensity,light0_intensity,4 * sizeof(GLfloat));
	GLfloat light0_ambient[]  = LIGHT0_AMBIENT;
	memcpy( light[0].ambient,  light0_ambient,  4 * sizeof(GLfloat));
	GLfloat light0_diffuse[]  = LIGHT0_DIFFUSE;
	memcpy( light[0].diffuse,  light0_diffuse,  4 * sizeof(GLfloat));
	GLfloat light0_specular[] = LIGHT0_SPECULAR;
	memcpy( light[0].specular, light0_specular, 4 * sizeof(GLfloat));
	GLfloat light0_position[] = LIGHT0_POSITION;
	memcpy( light[0].position, light0_position, 4 * sizeof(GLfloat));

	// Assign created components to GL_LIGHT0
	glLightfv(GL_LIGHT0, GL_AMBIENT,  light[0].ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  light[0].diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light[0].specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light[0].position);

	light[0].flag_ambient  = 1;
	light[0].flag_diffuse  = 1;
	light[0].flag_specular = 1;

	//init second light
	GLfloat light1_intensity[]  = LIGHT1_INTENSITY;
	memcpy( light[1].intensity,  light1_intensity,  4 * sizeof(GLfloat));
	GLfloat light1_ambient[]  = LIGHT1_AMBIENT;
	memcpy( light[1].ambient,  light1_ambient,  4 * sizeof(GLfloat));
	GLfloat light1_diffuse[]  = LIGHT1_DIFFUSE;
	memcpy( light[1].diffuse,  light1_diffuse,  4 * sizeof(GLfloat));
	GLfloat light1_specular[] = LIGHT1_SPECULAR;
	memcpy( light[1].specular, light1_specular, 4 * sizeof(GLfloat));
	GLfloat light1_position[] = LIGHT1_POSITION;
	memcpy( light[1].position, light1_position, 4 * sizeof(GLfloat));

	// Assign created components to GL_LIGHT1
	glLightfv(GL_LIGHT1, GL_AMBIENT,  light[1].ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE,  light[1].diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light[1].specular);
	glLightfv(GL_LIGHT1, GL_POSITION, light[1].position);

	light[1].flag_ambient  = 1;
	light[1].flag_diffuse  = 1;
	light[1].flag_specular = 1;

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_DEPTH_TEST);
}

/******************************************************************
* initialize
*******************************************************************/
void initialize(void){   
	glClearColor(BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B, 0.0);

	/* Enable depth testing */
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);    

	/* Setup vertex, color, and index buffer objects */
	init_objects();
	srand(time(NULL));
	init_object_mobile(root);

	for(int i=0; i < NUM_WALLS; i++) 
		init_object(walls[i]);

	init_object(billboard);

	/* Init Lighting */
	init_lights();

	/* Set projection transform */
	SetPerspectiveMatrix(FOVY, ASPECT, NEAR_PLANE, FAR_PLANE, proj_matrix);

	/* Set viewing transform */
	SetTranslation(0.0, 0.0, -1 * CAMERA_DIST, view_matrix);

	/* Init shaders and setup shader program */
	init_shaders();
}

/******************************************************************
* MouseInput
* 
* handler for mouse button pressed
*******************************************************************/
void mouse_input(int button, int state, int x, int y){
	if(button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		printf("CLICK @ x:%d y:%d button:%d\n",x,y,button);	
				
		root->obj.rotation_dir *= -1;
	}
}

/******************************************************************
* KeyInput
* 
* handler for keyboard-button pressed
*******************************************************************/
void key_input(unsigned char key, int x, int y){
	float rotation[16];
	float translte_camera[16];
	//float translte_board[16];
	GLfloat hsv[4] = {0.0, 1.0, 0.0, 1.0};
	
	printf("KEY  @ x:%d y:%d key:%c\n",x,y,key);		

	SetTranslation(0.0, 0.0, CAMERA_DIST, translte_camera);
	MultiplyMatrix(translte_camera, view_matrix, view_matrix);
	//SetTranslation(0.0, 0.0, CAMERA_DIST, translte_camera);
	//MultiplyMatrix(translte_board, billboard->model_matrix, billboard->model_matrix);

	switch(key) {
		case BTN_RIGHT: 	SetRotationY(CAMERA_ROTATE_ANGLE, rotation); 		 
					MultiplyMatrix(rotation, view_matrix, view_matrix); 
					SetRotationY(-CAMERA_ROTATE_ANGLE, rotation); 		 
					MultiplyMatrix(rotation, billboard->model_matrix, billboard->model_matrix);
					break;
		case BTN_LEFT: 		SetRotationY(360 - CAMERA_ROTATE_ANGLE, rotation); 
					MultiplyMatrix(rotation, view_matrix, view_matrix);
					SetRotationY(-(360 - CAMERA_ROTATE_ANGLE), rotation); 	
					MultiplyMatrix(rotation, billboard->model_matrix, billboard->model_matrix);
					break;
		case BTN_UP:		SetRotationX(CAMERA_ROTATE_ANGLE, rotation); 
					MultiplyMatrix(rotation, view_matrix, view_matrix); 
					break;
		case BTN_DOWN:		SetRotationX(360 - CAMERA_ROTATE_ANGLE, rotation); 
					MultiplyMatrix(rotation, view_matrix, view_matrix); 
					break;
		case BTN_TGL_SHADING:	if (shader_idx == GOURAUD_SHADER_CONST){
						shader_idx = PHONG_SHADER_CONST;
						printf("phong shading on\n");
					}else{
						shader_idx = GOURAUD_SHADER_CONST;
						printf("gouraud shading on\n");
					}
					glUseProgram(shader_program[shader_idx]);
					break;
		case BTN_HUE_UP:	RGBtoHSV(light[0].ambient, hsv);
					hsv[0] += 2.0f;
					if (hsv[0] > 360.0f) hsv[0] = 360.0f;
					HSVtoRGB(hsv, light[0].ambient);
					printf("hue up\n");
					break;
		case BTN_HUE_DOWN:	RGBtoHSV(light[0].ambient, hsv);
					hsv[0] -= 2.0f;
					if (hsv[0] < 0.0f) hsv[0] = 0.0f;
					HSVtoRGB(hsv, light[0].ambient);
					printf("hue down\n");
					break;
		case BTN_VALUE_UP:	RGBtoHSV(light[0].ambient, hsv);
					hsv[2] += 0.02f;
					if (hsv[2] > 1.0f) hsv[2] = 1.0f;
					HSVtoRGB(hsv, light[0].ambient);
					printf("value up\n");
					break;
		case BTN_VALUE_DOWN:	RGBtoHSV(light[0].ambient, hsv);
					hsv[2] -= 0.02f;
					if (hsv[2] < 0.0f) hsv[2] = 0.0f;
					HSVtoRGB(hsv, light[0].ambient);
					printf("value down\n");
					break;
		case BTN_TGL_AMBIENT:	if(light[0].flag_ambient || light[1].flag_ambient) {
						printf("ambient lighting off\n");
						light[0].flag_ambient = 0;
						light[1].flag_ambient = 0;
					} else {
						printf("ambient lighting on\n");
						light[0].flag_ambient = 1;
						light[1].flag_ambient = 1;
					}
					break;
		case BTN_TGL_DIFFUSE:	if(light[0].flag_diffuse || light[1].flag_diffuse) {
						printf("diffuse lighting off\n");
						light[0].flag_diffuse = 0;
						light[1].flag_diffuse = 0;
					} else {
						printf("diffuse lighting on\n");
						light[0].flag_diffuse = 1;
						light[1].flag_diffuse = 1;
					}
					break;
		case BTN_TGL_SPECULAR:	if(light[0].flag_specular || light[1].flag_specular) {
						printf("specular lighting off\n");
						light[0].flag_specular = 0;
						light[1].flag_specular = 0;
					} else {
						printf("specular lighting on\n");
						light[0].flag_specular = 1;
						light[1].flag_specular = 1;
					}
					break;
	};

	SetTranslation(0.0, 0.0,CAMERA_DIST * -1, translte_camera);
	MultiplyMatrix(translte_camera, view_matrix, view_matrix);
	//MultiplyMatrix(translte, billboard->model_matrix, billboard->model_matrix);
}

/******************************************************************
* free_memory
*
* frees allocated memory for node and subvariables
*******************************************************************/
void free_memory(node_object *node){
	if(node->child_l != NULL)
		free_memory(node->child_l);

	if(node->child_r != NULL)
		free_memory(node->child_r);

	free(node->obj.vertx_buffer_data);
	free(node->obj.normal_buffer_data);
	free(node->obj.color_buffer_data);
	free(node->obj.index_buffer_data);
	free(node);
}

/******************************************************************
* window close
*
* ON_WINDOW_CLOSE handler
*******************************************************************/
void window_close(){
	for(int i=0; i < NUM_WALLS; i++) {
		free(walls[i]);
	}

	free_memory(root);
}

/******************************************************************
* main
*******************************************************************/
int main(int argc, char** argv) {
	//init random
	srand(time(NULL));

	//init glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(400, 400);
	glutCreateWindow("CG ProgrammingExercise 3 - Eberharter/Haselwanter");

	//init glew
	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	//setup scene/objects
	initialize();

	//link handler functions
	glutIdleFunc(on_idle);
	glutDisplayFunc(display);
	glutMouseFunc(mouse_input);
	glutKeyboardUpFunc(key_input);
	glutCloseFunc(window_close);
	glutMainLoop();

	return EXIT_SUCCESS;
}
