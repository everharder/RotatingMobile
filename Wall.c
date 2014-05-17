#include "Wall.h"
#include "Matrix.h"
#include "ObjectGl.h"
#include <stdio.h>
#include <stdlib.h>


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
	grid->color_buffer_data = malloc(grid->num_vertx * 3 * sizeof(GLfloat));
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
object_gl* create_wall(float x1, float y1, float z1, float x2, float y2, float z2, float colorR, float colorG, float colorB){
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
	grid->vertx_buffer_data[ 8] = z2;

	grid->vertx_buffer_data[ 9] = x2;
	grid->vertx_buffer_data[10] = y1;
	grid->vertx_buffer_data[11] = z1;

	// Define color
	for (int i = 0; i < grid->num_vertx; i++){
		grid->color_buffer_data[3*i+0] = colorR;
		grid->color_buffer_data[3*i+1] = colorG;
		grid->color_buffer_data[3*i+2] = colorB;
	}

	// Define index
	grid->index_buffer_data[0] = 0;
	grid->index_buffer_data[1] = 1;
	grid->index_buffer_data[2] = 3;

	grid->index_buffer_data[3] = 1;
	grid->index_buffer_data[4] = 2;
	grid->index_buffer_data[5] = 3;

	return grid;
}
