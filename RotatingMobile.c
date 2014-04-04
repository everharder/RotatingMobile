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

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define BACKGROUND_COLOR	1.0, 1.0, 1.0, 0.0
#define FOVY			55.0
#define ASPECT			 3
#define NEAR_PLANE	 	 0.5
#define FAR_PLANE		50.0
#define CAMERA_DIST		20
#define CAMERA_ROTATE_ANGLE	 10
#define LEAF_ROTATION_SPD_MAX  	 0.5
#define LEAF_ROTATION_SPD_MIN  	 0.1

/******************************************************************
* globals
*******************************************************************/
GLuint ShaderProgram;
static const char* VertexShaderString;
static const char* FragmentShaderString;		

float ProjMatrix[16]; 	
float ViewMatrix[16]; 	

node_object *root;



void draw_mobile(node_object node){
	draw_single(node.obj, ProjMatrix, ViewMatrix, ShaderProgram);

	if(node.child_l != NULL)
		draw_mobile(*(node.child_l));

	if(node.child_r != NULL)
		draw_mobile(*(node.child_r));
}

/******************************************************************
* display
*******************************************************************/
void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	draw_mobile(*root);
	
	/* Swap between front and back buffer */ 
	glutSwapBuffers();
}

/******************************************************************
* do for tree
*
* use a given function on every node in a subtree defined by the
* rootnode {node}
*******************************************************************/
void do_for_tree(node_object *node, void (*op)(object_gl *, float), float f) {
	op(&(node->obj),f);

	if(node->child_l != NULL)
		do_for_tree(node->child_l, op, f);

	if(node->child_r != NULL)
		do_for_tree(node->child_r, op, f);
}

void rotate_mobile(node_object *node) {
	float now = glutGet(GLUT_ELAPSED_TIME);
	float time_delta = now - node->obj.rotation_dur;
	node->obj.rotation_dur = now;

	rotate_object(&(node->obj), time_delta * node->obj.rotation_spd * node->obj.rotation_dir);	

	if(node->child_l != NULL){
		do_for_tree(node->child_l, orbit_object, time_delta * node->obj.rotation_spd * node->obj.rotation_dir);
		rotate_mobile(node->child_l);
	}
	if(node->child_r != NULL){
		do_for_tree(node->child_r, orbit_object, time_delta * node->obj.rotation_spd * node->obj.rotation_dir);
		rotate_mobile(node->child_r);
	}
}

/******************************************************************
* on idle
*******************************************************************/
void on_idle(){
	rotate_mobile(root);

	/* Request redrawing forof window content */  
	glutPostRedisplay();
}

/******************************************************************
* add shader
*******************************************************************/
void add_shader(GLuint ShaderProgram, const char* ShaderCode, GLenum ShaderType){
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
	glAttachShader(ShaderProgram, ShaderObj);
}


/******************************************************************
* create shader program 
*******************************************************************/
void create_shader_program(){
	/* Allocate shader object */
	ShaderProgram = glCreateProgram();

	if (ShaderProgram == 0) {
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}

	/* Load shader code from file */
	VertexShaderString = LoadShader("vertexshader.vs");
	FragmentShaderString = LoadShader("fragmentshader.fs");

	/* Separately add vertex and fragment shader to program */
	add_shader(ShaderProgram, VertexShaderString, GL_VERTEX_SHADER);
	add_shader(ShaderProgram, FragmentShaderString, GL_FRAGMENT_SHADER);

	/* Link shader code into executable shader program */
	glLinkProgram(ShaderProgram);

	/* Check results of linking step */
	GLint Success = 0;
	GLchar ErrorLog[1024];
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	/* Check if shader program can be executed */ 
	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);

	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	/* Put linked shader program into drawing pipeline */
	glUseProgram(ShaderProgram);
}

/******************************************************************
* init objects
* 
* used to link the object arrays into the main arrays
*******************************************************************/
void init_objects() {
	root = parse_mobile("mobile.obj");

	if(root == NULL) {
		printf("error parsing file...\n");
		exit(EXIT_FAILURE);
	}
}

/******************************************************************
* setup buffer
*
* create buffer objects and load data into buffers
*******************************************************************/
void setup_buffer(node_object *node) {
	glGenBuffers(1, &(node->obj.vbo));
	glGenBuffers(1, &(node->obj.cbo));
	glGenBuffers(1, &(node->obj.ibo));

	glBindBuffer(GL_ARRAY_BUFFER, node->obj.vbo);
	glBufferData(GL_ARRAY_BUFFER, node->obj.num_vertx * 3 * sizeof(GLfloat), node->obj.vertx_buffer_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, node->obj.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, node->obj.num_vectr * node->obj.vertx_per_vectr * sizeof(GLushort), node->obj.index_buffer_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, node->obj.cbo);
	glBufferData(GL_ARRAY_BUFFER, node->obj.num_vertx * 3 * sizeof(GLfloat), node->obj.color_buffer_data, GL_STATIC_DRAW);
}

/******************************************************************
* init mobile
*******************************************************************/
void init_mobile(node_object *node) {
	setup_buffer(node);

	if(node->child_l != NULL)
		init_mobile(node->child_l);
	if(node->child_r != NULL)
		init_mobile(node->child_r);
}

/******************************************************************
* initialize
*******************************************************************/
void initialize(void){   
	glClearColor(BACKGROUND_COLOR);

	/* Enable depth testing */
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);    

	/* Setup vertex, color, and index buffer objects */
	init_objects();
	srand(time(NULL));
	init_mobile(root);

	/* Setup shaders and shader program */
	create_shader_program();

	/* Set projection transform */
	SetPerspectiveMatrix(FOVY, ASPECT, NEAR_PLANE, FAR_PLANE, ProjMatrix);

	/* Set viewing transform */
	SetTranslation(0.0, 0.0, -1 * CAMERA_DIST, ViewMatrix);
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
	float RotationMatrix[16];
	float TranslteMatrix[16];
	
	printf("KEY  @ x:%d y:%d key:%d\n",x,y,key);		

	if(key != 'w' && key != 'a' && key != 's' && key != 'd')
		return;

	SetTranslation(0.0, 0.0, CAMERA_DIST, TranslteMatrix);
	MultiplyMatrix(TranslteMatrix, ViewMatrix, ViewMatrix);

	switch(key) {
		case 'd': 	SetRotationY(CAMERA_ROTATE_ANGLE, RotationMatrix); 		 
				MultiplyMatrix(RotationMatrix, ViewMatrix, ViewMatrix); 
				break;
		case 'a': 	SetRotationY(360 - CAMERA_ROTATE_ANGLE, RotationMatrix); 
				MultiplyMatrix(RotationMatrix, ViewMatrix, ViewMatrix); 
				break;
		case 'w':	SetRotationX(CAMERA_ROTATE_ANGLE, RotationMatrix); 
				MultiplyMatrix(RotationMatrix, ViewMatrix, ViewMatrix); 
				break;
		case 's':	SetRotationX(360 - CAMERA_ROTATE_ANGLE, RotationMatrix); 
				MultiplyMatrix(RotationMatrix, ViewMatrix, ViewMatrix); 
				break;
	};

	SetTranslation(0.0, 0.0,CAMERA_DIST * -1, TranslteMatrix);
	MultiplyMatrix(TranslteMatrix, ViewMatrix, ViewMatrix); 
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
* window_close
*
* ON_WINDOW_CLOSE handler
*******************************************************************/
void window_close(){
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
