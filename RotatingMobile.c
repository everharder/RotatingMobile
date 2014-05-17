/******************************************************************
* PROGRAMMING EXERCISE 1 - Rotating Mobile
* authors: 	Daniel Eberharter
*	  	Stefan Haselwanter
* date: 	07.04.2014   
* version:	1.0
* desc.:	draws and animates a simple rotating mobile
		with the controls w, a, s, d the camera can be
		moved and the rotation direction of the top 
		wielding can be inverted by leftclick

		the code may contain fragments of the programming
		example no. 2 from Matthias Harders.
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
#include "Grid.h"

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
#define NUM_GRIDS		 3

#define BUTTON_UP		'w'
#define BUTTON_DOWN		's'
#define BUTTON_LEFT		'a'
#define BUTTON_RIGHT		'd'

//Lighting
#define LIGHTSOURCE_TYPE	GL_SPECULAR		

/******************************************************************
* globals
*******************************************************************/
GLuint shader_program;
static const char* VertexShaderString;
static const char* FragmentShaderString;		

float proj_matrix[16]; 	
float view_matrix[16]; 	

node_object *root;
object_gl *grids[NUM_GRIDS];	


/******************************************************************
* draw mobile
*******************************************************************/
void draw_mobile(node_object node){
	draw_single(&(node.obj), proj_matrix, view_matrix, shader_program);

	if(node.child_l != NULL)
		draw_mobile(*(node.child_l));

	if(node.child_r != NULL)
		draw_mobile(*(node.child_r));
}

/******************************************************************
* display
*******************************************************************/
void display(){
	//clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//draw grid
	draw_n(grids, NUM_GRIDS, proj_matrix, view_matrix, shader_program);

	//draw actual objects
	draw_mobile(*root);

	/* Swap between front and back buffer */ 
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
* on idle
*******************************************************************/
void on_idle(){
	rotate_mobile(root);
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
void create_shader_program(){
	/* Allocate shader object */
	shader_program = glCreateProgram();

	if (shader_program == 0) {
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}

	/* Load shader code from file */
	VertexShaderString = LoadShader("vertexshader.vs");
	FragmentShaderString = LoadShader("fragmentshader.fs");

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

	/* Put linked shader program into drawing pipeline */
	glUseProgram(shader_program);
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
	root = parse_mobile("mobile.obj");

	if(root == NULL) {
		printf("error parsing file...\n");
		exit(EXIT_FAILURE);
	}

	// Grids
	grids[0] = create_gridXY(-20.0, -10.0, -10.0, 20.0, 10.0, -10.0, 0.0, 0.0, 0.0, 20);
	grids[1] = create_gridXZ(-20.0, -10.0, -10.0, 20.0, -10.0, 10.0, 0.0, 0.0, 0.0, 20);
	grids[2] = create_gridYZ(-20.0, -10.0, -10.0, -20.0, 10.0, 10.0, 0.0, 0.0, 0.0, 20);

	if(grids[0] == NULL || grids[1] == NULL || grids[2] == NULL){
		printf("error creating grids...\n");
		exit(EXIT_FAILURE);
	}
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

	for(int i=0; i < NUM_GRIDS; i++) 
		init_object(grids[i]);

	/* Setup shaders and shader program */
	create_shader_program();

	/* Set projection transform */
	SetPerspectiveMatrix(FOVY, ASPECT, NEAR_PLANE, FAR_PLANE, proj_matrix);

	/* Set viewing transform */
	SetTranslation(0.0, 0.0, -1 * CAMERA_DIST, view_matrix);

	/* Init Lighting */

	// Create light components
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
        GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_position[] = { 1.0, 1.0, 0.0, 0.0 };

	glShadeModel(GL_SMOOTH); //gourard shading

	// Assign created components to GL_LIGHT0
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
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
	float translte[16];
	
	printf("KEY  @ x:%d y:%d key:%d\n",x,y,key);		

	if(key != BUTTON_UP && key != BUTTON_DOWN && key != BUTTON_LEFT && key != BUTTON_RIGHT)
		return;

	SetTranslation(0.0, 0.0, CAMERA_DIST, translte);
	MultiplyMatrix(translte, view_matrix, view_matrix);

	switch(key) {
		case BUTTON_RIGHT: 	SetRotationY(CAMERA_ROTATE_ANGLE, rotation); 		 
					MultiplyMatrix(rotation, view_matrix, view_matrix); 
					break;
		case BUTTON_LEFT: 	SetRotationY(360 - CAMERA_ROTATE_ANGLE, rotation); 
					MultiplyMatrix(rotation, view_matrix, view_matrix); 
					break;
		case BUTTON_UP:		SetRotationX(CAMERA_ROTATE_ANGLE, rotation); 
					MultiplyMatrix(rotation, view_matrix, view_matrix); 
					break;
		case BUTTON_DOWN:	SetRotationX(360 - CAMERA_ROTATE_ANGLE, rotation); 
					MultiplyMatrix(rotation, view_matrix, view_matrix); 
					break;
	};

	SetTranslation(0.0, 0.0,CAMERA_DIST * -1, translte);
	MultiplyMatrix(translte, view_matrix, view_matrix); 
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
	for(int i=0; i < NUM_GRIDS; i++) {
		free(grids[i]);
	}

	free_memory(root);
}

/******************************************************************
* main
*******************************************************************/
int main(int argc, char** argv) {
	//init glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(400, 400);
	glutCreateWindow("CG ProgrammingExercise 1 - Eberharter/Haselwanter");

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
