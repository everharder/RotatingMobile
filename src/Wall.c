#include "Wall.h"
#include "Matrix.h"
#include "ObjectGl.h"
#include <stdio.h>
#include <stdlib.h>
#include "Util.h"


/******************************************************************
* init_wall
*******************************************************************/
object_gl* init_wall(){
	object_gl *grid = calloc(1, sizeof(object_gl));
	grid->num_vertx = 4;
	grid->num_vectr = 2;
	grid->vertx_per_vectr = 3;
	grid->rotation_spd = 0;
	grid->rotation_dir = 1;
	grid->vertx_buffer_data = malloc(grid->num_vertx * 3 * sizeof(GLfloat));
	grid->normal_buffer_data = malloc(grid->num_vertx * grid->vertx_per_vectr * sizeof(GLfloat));
	grid->color_buffer_data = malloc(grid->num_vertx * 2 * sizeof(GLfloat));
	grid->index_buffer_data = malloc(grid->num_vectr * grid->vertx_per_vectr * sizeof(GLushort));
	SetIdentityMatrix(grid->model_matrix);

	return grid;
}

/******************************************************************
* create_wall
*
* creates a grid in direction of XY plane between diagonal points
* (x1, y1, z1) and (x2, y2, z2)
*******************************************************************/
object_gl* create_wallXY(float x1, float y1, float z1, float x2, float y2, float z2, char *texture){
	object_gl* grid = NULL;

	// Initialize a new grid
	grid = init_wall();

	if (grid == NULL) 
		return NULL;

	// Define vertexes
	grid->vertx_buffer_data[ 0] = x1;
	grid->vertx_buffer_data[ 1] = y1;
	grid->vertx_buffer_data[ 2] = z1;

	grid->vertx_buffer_data[ 3] = x2;
	grid->vertx_buffer_data[ 4] = y2;
	grid->vertx_buffer_data[ 5] = z2;

	grid->vertx_buffer_data[ 6] = x1;
	grid->vertx_buffer_data[ 7] = y2;
	grid->vertx_buffer_data[ 8] = z1;

	grid->vertx_buffer_data[ 9] = x2;
	grid->vertx_buffer_data[10] = y1;
	grid->vertx_buffer_data[11] = z1;

	// Define normals
	for (int i = 0; i < grid->num_vertx; i++){
		grid->normal_buffer_data[i*3 + 0] = 0.0;
		grid->normal_buffer_data[i*3 + 1] = 0.0;
		grid->normal_buffer_data[i*3 + 2] = 1.0;
	}

	setup_wall_texture(grid,texture);

	// Define index
	grid->index_buffer_data[0] = 0;
	grid->index_buffer_data[1] = 2;
	grid->index_buffer_data[2] = 3;

	grid->index_buffer_data[3] = 1;
	grid->index_buffer_data[4] = 2;
	grid->index_buffer_data[5] = 3;

	return grid;
}

/******************************************************************
* create_wall
*
* creates a grid in direction of XY plane between diagonal points
* (x1, y1, z1) and (x2, y2, z2)
*******************************************************************/
object_gl* create_wallXZ(float x1, float y1, float z1, float x2, float y2, float z2, char *texture){
	object_gl* grid = NULL;

	// Initialize a new grid
	grid = init_wall();

	if (grid == NULL) 
		return NULL;

	// Define vertexes
	grid->vertx_buffer_data[ 0] = x1;
	grid->vertx_buffer_data[ 1] = y1;
	grid->vertx_buffer_data[ 2] = z1;

	grid->vertx_buffer_data[ 3] = x2;
	grid->vertx_buffer_data[ 4] = y2;
	grid->vertx_buffer_data[ 5] = z2;

	grid->vertx_buffer_data[ 6] = x2;
	grid->vertx_buffer_data[ 7] = y1;
	grid->vertx_buffer_data[ 8] = z1;

	grid->vertx_buffer_data[ 9] = x1;
	grid->vertx_buffer_data[10] = y1;
	grid->vertx_buffer_data[11] = z2;

	// Define normals
	for (int i = 0; i < grid->num_vertx; i++){
		grid->normal_buffer_data[i*3 + 0] = 0.0;
		grid->normal_buffer_data[i*3 + 1] = 1.0;
		grid->normal_buffer_data[i*3 + 2] = 0.0;
	}

	setup_wall_texture(grid, texture);

	// Define index
	grid->index_buffer_data[0] = 0;
	grid->index_buffer_data[1] = 2;
	grid->index_buffer_data[2] = 3;

	grid->index_buffer_data[3] = 1;
	grid->index_buffer_data[4] = 2;
	grid->index_buffer_data[5] = 3;

	return grid;
}

/******************************************************************
* create_wall
*
* creates a grid in direction of XY plane between diagonal points
* (x1, y1, z1) and (x2, y2, z2)
*******************************************************************/
object_gl* create_wallYZ(float x1, float y1, float z1, float x2, float y2, float z2, char *texture){
	object_gl* grid = NULL;

	// Initialize a new grid
	grid = init_wall();

	if (grid == NULL) 
		return NULL;

	// Define vertexes
	grid->vertx_buffer_data[ 0] = x1;
	grid->vertx_buffer_data[ 1] = y1;
	grid->vertx_buffer_data[ 2] = z1;

	grid->vertx_buffer_data[ 3] = x2;
	grid->vertx_buffer_data[ 4] = y2;
	grid->vertx_buffer_data[ 5] = z2;

	grid->vertx_buffer_data[ 6] = x1;
	grid->vertx_buffer_data[ 7] = y1;
	grid->vertx_buffer_data[ 8] = z2;

	grid->vertx_buffer_data[ 9] = x1;
	grid->vertx_buffer_data[10] = y2;
	grid->vertx_buffer_data[11] = z1;

	// Define normals
	for (int i = 0; i < grid->num_vertx; i++){
		grid->normal_buffer_data[i*3 + 0] = 1.0;
		grid->normal_buffer_data[i*3 + 1] = 0.0;
		grid->normal_buffer_data[i*3 + 2] = 0.0;
	}

	setup_wall_texture(grid, texture);

	// Define index
	grid->index_buffer_data[0] = 0;
	grid->index_buffer_data[1] = 2;
	grid->index_buffer_data[2] = 3;

	grid->index_buffer_data[3] = 1;
	grid->index_buffer_data[4] = 2;
	grid->index_buffer_data[5] = 3;

	return grid;
}

void setup_wall_texture(object_gl *grid, char *texture) {
	// Define color and normals
	grid->color_buffer_data[0] = 0.0;
	grid->color_buffer_data[1] = 0.0;

	grid->color_buffer_data[2] = 1.0;
	grid->color_buffer_data[3] = 0.0;

	grid->color_buffer_data[4] = 0.0;
	grid->color_buffer_data[5] = 1.0;

	grid->color_buffer_data[6] = 1.0;
	grid->color_buffer_data[7] = 1.0;

	/* Allocate texture container */
   	grid->texture = malloc(sizeof(TextureDataPtr));
	int success = LoadTexture(texture, grid->texture);
	if (!success)
	{
	printf("Error loading texture '%s'. Exiting.\n", texture);
	exit(-1);
	}

	/* Create texture name and store in handle */
	glGenTextures(1, &(grid->texture_id));

	/* Bind texture */
	glBindTexture(GL_TEXTURE_2D, grid->texture_id);

	/* Load texture image into memory */
	glTexImage2D(GL_TEXTURE_2D,     /* Target texture */
		 0,                 /* Base level */
		 GL_RGB,            /* Each element is RGB triple */ 
		 grid->texture->width, grid->texture->height, 
		 0,                 /* Border should be zero */
		 GL_BGR,            /* Data storage format */
		 GL_UNSIGNED_BYTE,  /* Type of pixel data */
		 grid->texture->data);    /* Pointer to image data  */

	/* Repeat texture on edges when tiling */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	/* Linear interpolation for magnification */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* Trilinear MIP mapping for minification */ 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
	glGenerateMipmap(GL_TEXTURE_2D); 
}
