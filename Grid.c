#include "Grid.h"
#include "Matrix.h"
#include "ObjectGl.h"
#include <stdio.h>
#include <stdlib.h>


/******************************************************************
* init_grid
*******************************************************************/
object_gl* init_grid(int vertices){
	object_gl *grid = calloc(1, sizeof(object_gl));
	grid->num_vertx = 4*vertices;
	grid->num_vectr = 2*vertices;
	grid->vertx_per_vectr = 2;
	grid->rotation_spd = 0;
	grid->rotation_dir = 1;
	grid->vertx_buffer_data = malloc(grid->num_vertx * 3 * sizeof(GLfloat));
	grid->color_buffer_data = malloc(grid->num_vertx * 3 * sizeof(GLfloat));
	grid->index_buffer_data = malloc(grid->num_vectr * grid->vertx_per_vectr * sizeof(GLushort));
	SetIdentityMatrix(grid->model_matrix);

	return grid;
}

/******************************************************************
* create_gridXY
*
* creates a grid in direction of XY plane between diagonal points
* (x1, y1, z1) and (x2, y2, z2)
*******************************************************************/
object_gl* create_gridXY(float x1, float y1, float z1, float x2, float y2, float z2, float colorR, float colorG, float colorB, int vertices){
	float distX = (x2-x1)/vertices;
	float distY = (y2-y1)/vertices;
	object_gl* grid = NULL;

	// Initialize a new grid
	grid = init_grid(vertices);

	if (grid == NULL) return NULL;

	// Define vertex on x axis
	for (int i = 0; i < vertices; i++){
		// Down
		grid->vertx_buffer_data[6*i+0] = x1 + i*distX;
		grid->vertx_buffer_data[6*i+1] = y1;
		grid->vertx_buffer_data[6*i+2] = z1;
		// Up
		grid->vertx_buffer_data[6*i+3] = x1 + i*distX;
		grid->vertx_buffer_data[6*i+4] = y2;
		grid->vertx_buffer_data[6*i+5] = z2;
	}

	// Define vertex on y axis
	for (int i = 0; i < vertices; i++){
		// Left
		grid->vertx_buffer_data[6*(i+vertices)+0] = x1;
		grid->vertx_buffer_data[6*(i+vertices)+1] = y1 + i*distY;
		grid->vertx_buffer_data[6*(i+vertices)+2] = z1;
		// Right
		grid->vertx_buffer_data[6*(i+vertices)+3] = x2;
		grid->vertx_buffer_data[6*(i+vertices)+4] = y1 + i*distY;
		grid->vertx_buffer_data[6*(i+vertices)+5] = z2;
	}
	// Define color
	for (int i = 0; i < grid->num_vertx; i++){
		grid->color_buffer_data[3*i+0] = colorR;
		grid->color_buffer_data[3*i+1] = colorG;
		grid->color_buffer_data[3*i+2] = colorB;
	}
	// Define index
	int j = 0;
	for (int i = 0; i < grid->num_vertx; i+=2){
		grid->index_buffer_data[grid->vertx_per_vectr*j+0] = i;
		grid->index_buffer_data[grid->vertx_per_vectr*j+1] = i+1;
		j++;
	}

	return grid;
}

/******************************************************************
* create_gridXZ
*
* creates a grid in direction of XZ plane between diagonal points
* (x1, y1, z1) and (x2, y2, z2)
*******************************************************************/
object_gl* create_gridXZ(float x1, float y1, float z1, float x2, float y2, float z2, float colorR, float colorG, float colorB, int vertices){
	float distX = (x2-x1)/vertices;
	float distZ = (z2-z1)/vertices;
	object_gl* grid = NULL;

	// Initialize a new grid
	grid = init_grid(vertices);

	if (grid == NULL) return NULL;

	// Define vertex on x axis
	for (int i = 0; i < vertices; i++){
		// Down
		grid->vertx_buffer_data[6*i+0] = x1 + i*distX;
		grid->vertx_buffer_data[6*i+1] = y1;
		grid->vertx_buffer_data[6*i+2] = z1;
		// Up
		grid->vertx_buffer_data[6*i+3] = x1 + i*distX;
		grid->vertx_buffer_data[6*i+4] = y2;
		grid->vertx_buffer_data[6*i+5] = z2;
	}
	// Define vertex on z axis
	for (int i = 0; i < vertices; i++){
		// Left
		grid->vertx_buffer_data[6*(i+vertices)+0] = x1;
		grid->vertx_buffer_data[6*(i+vertices)+1] = y1;
		grid->vertx_buffer_data[6*(i+vertices)+2] = z1 + i*distZ;
		// Right
		grid->vertx_buffer_data[6*(i+vertices)+3] = x2;
		grid->vertx_buffer_data[6*(i+vertices)+4] = y2;
		grid->vertx_buffer_data[6*(i+vertices)+5] = z1 + i*distZ;
	}
	// Define color
	for (int i = 0; i < grid->num_vertx; i++){
		grid->color_buffer_data[3*i+0] = colorR;
		grid->color_buffer_data[3*i+1] = colorG;
		grid->color_buffer_data[3*i+2] = colorB;
	}
	// Define index
	int j = 0;
	for (int i = 0; i < grid->num_vertx; i+=2){
		grid->index_buffer_data[grid->vertx_per_vectr*j+0] = i;
		grid->index_buffer_data[grid->vertx_per_vectr*j+1] = i+1;
		j++;
	}

	return grid;
}

/******************************************************************
* create_gridYZ
*
* creates a grid in direction of YZ plane between diagonal points
* (x1, y1, z1) and (x2, y2, z2)
*******************************************************************/
object_gl* create_gridYZ(float x1, float y1, float z1, float x2, float y2, float z2, float colorR, float colorG, float colorB, int vertices){
	float distY = (y2-y1)/vertices;
	float distZ = (z2-z1)/vertices;
	object_gl* grid = NULL;

	// Initialize a new grid
	grid = init_grid(vertices);

	if (grid == NULL) return NULL;

	// Define vertex on z axis
	for (int i = 0; i < vertices; i++){
		// Down
		grid->vertx_buffer_data[6*i+0] = x1;
		grid->vertx_buffer_data[6*i+1] = y1;
		grid->vertx_buffer_data[6*i+2] = z1 + i*distZ;
		// Up
		grid->vertx_buffer_data[6*i+3] = x2;
		grid->vertx_buffer_data[6*i+4] = y2;
		grid->vertx_buffer_data[6*i+5] = z1 + i*distZ;
	}
	// Define vertex on y axis
	for (int i = 0; i < vertices; i++){
		// Left
		grid->vertx_buffer_data[6*(i+vertices)+0] = x1;
		grid->vertx_buffer_data[6*(i+vertices)+1] = y1 + i*distY;
		grid->vertx_buffer_data[6*(i+vertices)+2] = z1;
		// Right
		grid->vertx_buffer_data[6*(i+vertices)+3] = x2;
		grid->vertx_buffer_data[6*(i+vertices)+4] = y1 + i*distY;
		grid->vertx_buffer_data[6*(i+vertices)+5] = z2;
	}
	// Define color
	for (int i = 0; i < grid->num_vertx; i++){
		grid->color_buffer_data[3*i+0] = colorR;
		grid->color_buffer_data[3*i+1] = colorG;
		grid->color_buffer_data[3*i+2] = colorB;
	}
	// Define index
	int j = 0;
	for (int i = 0; i < grid->num_vertx; i+=2){
		grid->index_buffer_data[grid->vertx_per_vectr*j+0] = i;
		grid->index_buffer_data[grid->vertx_per_vectr*j+1] = i+1;
		j++;
	}

	return grid;
}

/******************************************************************
* parse_grid3D
*
* parses 3 grids to one 3D object
*******************************************************************/
node_object* parse_grid3D(object_gl objX, object_gl objY, object_gl objZ){
	node_object *grids;

	grids = calloc(1, sizeof(node_object));
	grids->child_r = calloc(1, sizeof(node_object));
	grids->child_l = calloc(1, sizeof(node_object));

	grids->obj = objX;
	grids->child_l->obj = objY;
	grids->child_r->obj = objZ;

	return grids;
}
