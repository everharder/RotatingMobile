#include <stdio.h>
#include <stdlib.h>
#include "Billboard.h"
#include "Matrix.h"
#include "ObjectGl.h"
#include "Util.h"


/******************************************************************
* init_board
*******************************************************************/
object_gl* init_board(){
	object_gl *board = calloc(1, sizeof(object_gl));
	board->num_vertx = 4;
	board->num_vectr = 2;
	board->vertx_per_vectr = 3;
	board->rotation_spd = 0;
	board->rotation_dir = 1;
	board->alpha = 0.75;
	board->vertx_buffer_data = malloc(board->num_vertx * 3 * sizeof(GLfloat));
	board->normal_buffer_data = malloc(board->num_vertx * board->vertx_per_vectr * sizeof(GLfloat));
	board->color_buffer_data = malloc(board->num_vertx * 2 * sizeof(GLfloat));
	board->index_buffer_data = malloc(board->num_vectr * board->vertx_per_vectr * sizeof(GLushort));
	SetIdentityMatrix(board->model_matrix);

	return board;
}

/******************************************************************
* create_board
*
* creates a bill board in direction of XY plane between diagonal points
* (x1, y1, z1) and (x2, y2, z2)
*******************************************************************/
object_gl* create_board(float x1, float y1, float z1, float x2, float y2, float z2){
	object_gl* board = NULL;

	// Initialize a new board
	board = init_board();

	if (board == NULL) 
		return NULL;

	// Define vertexes
	board->vertx_buffer_data[ 0] = x1;
	board->vertx_buffer_data[ 1] = y1;
	board->vertx_buffer_data[ 2] = z1;

	board->vertx_buffer_data[ 3] = x2;
	board->vertx_buffer_data[ 4] = y2;
	board->vertx_buffer_data[ 5] = z2;

	board->vertx_buffer_data[ 6] = x1;
	board->vertx_buffer_data[ 7] = y2;
	board->vertx_buffer_data[ 8] = z1;

	board->vertx_buffer_data[ 9] = x2;
	board->vertx_buffer_data[10] = y1;
	board->vertx_buffer_data[11] = z1;

	// Define normals
	for (int i = 0; i < board->num_vertx; i++){
		board->normal_buffer_data[i*3 + 0] = 0.0;
		board->normal_buffer_data[i*3 + 1] = 0.0;
		board->normal_buffer_data[i*3 + 2] = 1.0;
	}

	// Define UV coordinates for each vertex
	board->color_buffer_data[0] = 0.0 + UV_OFFSET;
	board->color_buffer_data[1] = 0.66;

	board->color_buffer_data[2] = 0.25 + UV_OFFSET;
	board->color_buffer_data[3] = 0.99;

	board->color_buffer_data[4] = 0.0 + UV_OFFSET;
	board->color_buffer_data[5] = 0.99;

	board->color_buffer_data[6] = 0.25 + UV_OFFSET;
	board->color_buffer_data[7] = 0.66;

	// Define index
	board->index_buffer_data[0] = 0;
	board->index_buffer_data[1] = 2;
	board->index_buffer_data[2] = 3;

	board->index_buffer_data[3] = 1;
	board->index_buffer_data[4] = 2;
	board->index_buffer_data[5] = 3;

	// Load texture
	setup_texture(board, "data/dance.bmp");

	return board;
}
