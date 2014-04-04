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

#define NUM_LEAFS		 3
#define NUM_WIELD		 2
#define NUM_LAYERS		 3
#define NUM_GRIDS		 3
#define GRID_SIZE		 20.0
#define VERTICES_PER_GRID	 10
#define IDX_ROOT_WIELDING	 3
#define CAMERA_DIST		30
#define CAMERA_ROTATE_ANGLE	 5
#define LEAF_ROTATION_SPD_MAX  0.5
#define LEAF_ROTATION_SPD_MIN  0.1

/******************************************************************
* globals
*******************************************************************/
GLuint ShaderProgram;

static const char* VertexShaderString;
static const char* FragmentShaderString;		

float ProjMatrix[16]; 	
float ViewMatrix[16]; 	

node_object *root;
node_object grid;			// currently not in use!
node_object *grids[NUM_GRIDS];		

/******************************************************************
* Wall
*******************************************************************/
GLfloat wall_vertx[] = {
	-20.0, -10.0, -10.0,
	 20.0, -10.0, -10.0,
	 20.0,  10.0, -10.0,
	-20.0,  10.0, -10.0,
	-20.0, -10.0,  10.0,
	-20.0,  10.0,  10.0,
	 20.0, -10.0,  10.0
};

GLfloat wall_color[] = {
    1.0, 1.0, 1.0,
    0.5, 0.5, 0.5,
    1.0, 1.0, 1.0,
    0.5, 0.5, 0.5,
    0.5, 0.5, 0.5,
    1.0, 1.0, 1.0,
    1.0, 1.0, 1.0,
};

GLushort wall_index[] = {
	0, 1, 2,
	0, 2, 3,
	0, 3, 5,
	0, 4, 5,
	0, 1, 6,
	0, 4, 6
};



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

	for (int i = 0; i < NUM_GRIDS; i++)
		draw_single(grids[i]->obj, ProjMatrix, ViewMatrix, ShaderProgram);
	//draw_single(grid.obj, ProjMatrix, ViewMatrix, ShaderProgram);
	
	/* Swap between front and back buffer */ 
	glutSwapBuffers();
}


/******************************************************************
* do for tree
*
* use a given function on every node in a subtree defined by the
* rootnode {node}
*******************************************************************/
void do_for_tree(node_object *node, void (*op)(object_gl *, float, float, float, float), float f1, float f2, float f3, float f4) {
	op(&(node->obj),f1,f2,f3,f4);

	if(node->child_l != NULL)
		do_for_tree(node->child_l, op, f1, f2, f3, f4);

	if(node->child_r != NULL)
		do_for_tree(node->child_r, op, f1, f2, f3, f4);
}


void rotate_mobile(node_object *node) {
	if(node->child_l != NULL || node->child_r != NULL) {
		do_for_tree(node, orbit_object, (glutGet(GLUT_ELAPSED_TIME) - node->obj.rotation_dur) * node->obj.rotation_spd * node->obj.rotation_dir, node->obj.x, node->obj.y, node->obj.z);

		node->obj.rotation_dur = glutGet(GLUT_ELAPSED_TIME);

		if(node->child_l != NULL)
			rotate_mobile(node->child_l);
		if(node->child_r != NULL)
			rotate_mobile(node->child_r);
	} else {
		rotate_object(&(node->obj), (glutGet(GLUT_ELAPSED_TIME) - node->obj.rotation_dur) * node->obj.rotation_spd * node->obj.rotation_dir);
	
		node->obj.rotation_dur = glutGet(GLUT_ELAPSED_TIME);
	}
}

/******************************************************************
* on idle
*******************************************************************/
void on_idle(){
	//do_for_tree_2(&(mobile[IDX_ROOT_WIELDING]), rotate_object_native);
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

	GLint Success = 0;
	GLchar ErrorLog[1024];

	/* Link shader code into executable shader program */
	glLinkProgram(ShaderProgram);

	/* Check results of linking step */
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
* create grid
*
* create grids
*******************************************************************/
void create_grid(int num, int vertices){
	GLfloat dist = GRID_SIZE/vertices;

	// Define vertex on x axis
	for (int i = 0; i < vertices; i++){
		// Down
		grids[num]->obj.vertx_buffer_data[6*i+0] = -GRID_SIZE + i*dist;
		grids[num]->obj.vertx_buffer_data[6*i+1] = -GRID_SIZE/2;
		grids[num]->obj.vertx_buffer_data[6*i+2] = -GRID_SIZE/2;
		// Up
		grids[num]->obj.vertx_buffer_data[6*i+3] = -GRID_SIZE + i*dist;
		grids[num]->obj.vertx_buffer_data[6*i+4] = GRID_SIZE-dist;
		grids[num]->obj.vertx_buffer_data[6*i+5] = -GRID_SIZE/2;
	}
	// Define vertex on y axis
	for (int i = 0; i < vertices; i++){
		// Left
		grids[num]->obj.vertx_buffer_data[6*(i+vertices)+0] = -GRID_SIZE;
		grids[num]->obj.vertx_buffer_data[6*(i+vertices)+1] = -GRID_SIZE/2 + i*dist;
		grids[num]->obj.vertx_buffer_data[6*(i+vertices)+2] = -GRID_SIZE/2;
		// Right
		grids[num]->obj.vertx_buffer_data[6*(i+vertices)+3] = -GRID_SIZE;
		grids[num]->obj.vertx_buffer_data[6*(i+vertices)+4] = -GRID_SIZE/2 + i*dist;
		grids[num]->obj.vertx_buffer_data[6*(i+vertices)+5] = -GRID_SIZE/2;
	}
	// Define color
	for (int i = 0; i < grids[num]->obj.num_vertx; i++){
		grids[num]->obj.color_buffer_data[3*i+0] = 0.0;
		grids[num]->obj.color_buffer_data[3*i+1] = 0.0;
		grids[num]->obj.color_buffer_data[3*i+2] = 0.0;
	}
	// Define index
	int j = 0;
	for (int i = 0; i < grids[num]->obj.num_vertx; i+=2){
		grids[num]->obj.index_buffer_data[grids[num]->obj.vertx_per_vectr*j+0] = i;
		grids[num]->obj.index_buffer_data[grids[num]->obj.vertx_per_vectr*j+1] = i+1;
		j++;
	}
}

/******************************************************************
* init node
*
* fills a single node with the necessary information
*******************************************************************/
void init_node(node_object *node) {
	setup_buffer(node);
	/* Initialize matrices */
	SetIdentityMatrix(node->obj.model_matrix);
}

void init_mobile(node_object *node) {
	init_node(node);

	if(node->child_l != NULL)
		init_mobile(node->child_l);
	if(node->child_r != NULL)
		init_mobile(node->child_r);
}

void init_grid(int vertices){
	float RotationMatrix[16];

/*
	grid.obj.num_vertx = 7;
	grid.obj.num_vectr = 6;
	grid.obj.vertx_per_vectr = 3;
	grid.obj.rotation_spd = 0;
	grid.obj.rotation_dir = 1;
	grid.obj.vertx_buffer_data = wall_vertx;
	grid.obj.color_buffer_data = wall_color;
	grid.obj.index_buffer_data = wall_index;

	init_node(&grid);
*/

	for (int i = 0; i < NUM_GRIDS; i++){
		grids[i] = calloc(1, sizeof(node_object));
		grids[i]->obj.num_vertx = 4*vertices;
		grids[i]->obj.num_vectr = 2*vertices;
		grids[i]->obj.vertx_per_vectr = 2;
		grids[i]->obj.rotation_spd = 0;
		grids[i]->obj.rotation_dir = 1;
		grids[i]->obj.vertx_buffer_data = malloc(grids[i]->obj.num_vertx * 3 * sizeof(GLfloat));
		grids[i]->obj.color_buffer_data = malloc(grids[i]->obj.num_vertx * 3 * sizeof(GLfloat));
		grids[i]->obj.index_buffer_data = malloc(grids[i]->obj.num_vectr * grids[i]->obj.vertx_per_vectr * sizeof(GLushort));

		create_grid(i, vertices);
		init_node(grids[i]);
	}

	SetRotationX(270, RotationMatrix);
	MultiplyMatrix(RotationMatrix, grids[1]->obj.model_matrix, grids[1]->obj.model_matrix);

	SetRotationY(90, RotationMatrix);
	MultiplyMatrix(RotationMatrix, grids[2]->obj.model_matrix, grids[2]->obj.model_matrix);
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

	init_grid(VERTICES_PER_GRID);
}

/******************************************************************
* initialize
*******************************************************************/
void initialize(void){   
	glClearColor(1.0, 1.0, 1.0, 0.0);

	/* Enable depth testing */
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);    

	/* Setup vertex, color, and index buffer objects */
	init_objects();
	srand(time(NULL));
	init_mobile(root);

	/* Setup shaders and shader program */
	create_shader_program();

	SetIdentityMatrix(ProjMatrix);
	SetIdentityMatrix(ViewMatrix);

	/* Set projection transform */
	float fovy = 55.0;
	float aspect = 3; 
	float nearPlane = 0.5; 
	float farPlane = 50.0;
	SetPerspectiveMatrix(fovy, aspect, nearPlane, farPlane, ProjMatrix);

	/* Set viewing transform */
	float camera_disp = -1 * CAMERA_DIST;
	SetTranslation(0.0, 0.0, camera_disp, ViewMatrix);
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
		case 'a': 	SetRotationY(CAMERA_ROTATE_ANGLE * -1, RotationMatrix); 
				MultiplyMatrix(RotationMatrix, ViewMatrix, ViewMatrix); 
				break;
		case 'w':	SetRotationX(CAMERA_ROTATE_ANGLE, RotationMatrix); 
				MultiplyMatrix(RotationMatrix, ViewMatrix, ViewMatrix); 
				break;
		case 's':	SetRotationX(CAMERA_ROTATE_ANGLE * -1, RotationMatrix); 
				MultiplyMatrix(RotationMatrix, ViewMatrix, ViewMatrix); 
				break;
	};

	SetTranslation(0.0, 0.0,CAMERA_DIST * -1, TranslteMatrix);
	MultiplyMatrix(TranslteMatrix, ViewMatrix, ViewMatrix); 
}

void free_memory(node_object *node){
	if(node->child_l != NULL)
		free_memory(node->child_l);

	if(node->child_r != NULL)
		free_memory(node->child_r);

	free(node->obj.vertx_buffer_data);
	free(node->obj.color_buffer_data);
	free(node->obj.index_buffer_data);
	free(node);
	free(grids[0]);
	free(grids[1]);
	free(grids[2]);
}

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
	glutCreateWindow("CG ProgrammingExercise 1 - Eberharter");

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

	return 0;
}
