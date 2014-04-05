#include "OBJParserMobile.h"
#include "Matrix.h"
#include <stdio.h>
#include <stdlib.h>

#define _DEBUG__DEBUG_OBJPARSER_

node_object* parse_mobile(char *filename) {
	node_object **nodes = NULL;
	node_object *root = NULL;
	int  root_idx = 0, child_idx = 0, num_objects = 0, i_buff = 0; 
	FILE *fp;

	//open file
	if((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "cannot find file %s\n", filename);
		exit(EXIT_FAILURE);
	}

	//read num_objects
	if(fscanf(fp, "%d\n", &i_buff) < 1){
		fprintf(stderr, "error parsing file %s\n", filename);
		exit(EXIT_FAILURE);
	}
	num_objects = i_buff;
	
	//read root_idx
	if(fscanf(fp, "%d\n", &i_buff) < 1){
		fprintf(stderr, "error parsing file %s\n", filename);
		exit(EXIT_FAILURE);
	}
	root_idx = i_buff;

	#ifdef _DEBUG__DEBUG_OBJPARSER_
		printf("num_obj: %d\nroot_idx: %d\n",num_objects,root_idx);
	#endif

	nodes = calloc(num_objects, sizeof(node_object*));
	for(int curr_obj=0; curr_obj < num_objects; curr_obj++) {
		nodes[curr_obj] = calloc(1, sizeof(node_object));

		fscanf(fp, "\n%d\n",&nodes[curr_obj]->obj.num_vertx);
		fscanf(fp, "%d\n",  &nodes[curr_obj]->obj.num_vectr);
		fscanf(fp, "%d\n",  &nodes[curr_obj]->obj.vertx_per_vectr);
		fscanf(fp, "%f\n",  &nodes[curr_obj]->obj.rotation_spd);
		fscanf(fp, "%hd\n",  &nodes[curr_obj]->obj.rotation_dir);
		#ifdef _DEBUG__DEBUG_OBJPARSER_
			printf("\n\nnum_vertx: %d\nnum_vectr: %d\nvertx_per_vectr: %d\n", nodes[curr_obj]->obj.num_vertx,
											  nodes[curr_obj]->obj.num_vectr,
											  nodes[curr_obj]->obj.vertx_per_vectr);
			printf("rotation_spd: %f\nrotation_dir: %d\n",  nodes[curr_obj]->obj.rotation_spd, 
									nodes[curr_obj]->obj.rotation_dir);
		#endif

		nodes[curr_obj]->obj.vertx_buffer_data = malloc(nodes[curr_obj]->obj.num_vertx * 3 * sizeof(GLfloat));
		nodes[curr_obj]->obj.color_buffer_data = malloc(nodes[curr_obj]->obj.num_vertx * 3 * sizeof(GLfloat));
		nodes[curr_obj]->obj.index_buffer_data = malloc(nodes[curr_obj]->obj.num_vectr * nodes[curr_obj]->obj.vertx_per_vectr * sizeof(GLushort));

		//read vertex data
		for(int i=0; i < nodes[curr_obj]->obj.num_vertx; i++) {
			fscanf(fp,"v %f %f %f\n", &nodes[curr_obj]->obj.vertx_buffer_data[3*i + 0], 
						   &nodes[curr_obj]->obj.vertx_buffer_data[3*i + 1],
						   &nodes[curr_obj]->obj.vertx_buffer_data[3*i + 2]);
		}
		#ifdef _DEBUG__DEBUG_OBJPARSER_
			print_matrix("vertex data", nodes[curr_obj]->obj.vertx_buffer_data, 3, nodes[curr_obj]->obj.num_vertx);
		#endif

		//read color data
		for(int i=0; i < nodes[curr_obj]->obj.num_vertx; i++) {
			fscanf(fp,"c %f %f %f\n", &nodes[curr_obj]->obj.color_buffer_data[3*i + 0], 
						   &nodes[curr_obj]->obj.color_buffer_data[3*i + 1],
						   &nodes[curr_obj]->obj.color_buffer_data[3*i + 2]);
		}
		#ifdef _DEBUG__DEBUG_OBJPARSER_
			print_matrix("color data", nodes[curr_obj]->obj.color_buffer_data, 3, nodes[curr_obj]->obj.num_vertx);
		#endif

		//read index data
		for(int i=0; i < nodes[curr_obj]->obj.num_vectr; i++) {
			fscanf(fp,"i");
			for(int j=0; j < nodes[curr_obj]->obj.vertx_per_vectr; j++) {
				fscanf(fp," %hu", &nodes[curr_obj]->obj.index_buffer_data[nodes[curr_obj]->obj.vertx_per_vectr*i + j]);
			}
			fscanf(fp,"\n");
		}
		#ifdef _DEBUG__DEBUG_OBJPARSER_
			print_matrix_ushort("index data", nodes[curr_obj]->obj.index_buffer_data, nodes[curr_obj]->obj.vertx_per_vectr, nodes[curr_obj]->obj.num_vectr);
		#endif

		//read center
		fscanf(fp,"center %f %f %f\n", 	&nodes[curr_obj]->obj.x,
						&nodes[curr_obj]->obj.y,
						&nodes[curr_obj]->obj.z);
		#ifdef _DEBUG__DEBUG_OBJPARSER_
		printf("\n\ncenter_x: %f\ncenter_y: %f\ncenter_z: %f\n",  nodes[curr_obj]->obj.x,
									  nodes[curr_obj]->obj.y,
									  nodes[curr_obj]->obj.z);
		#endif

		float transl[16];
		SetTranslation(nodes[curr_obj]->obj.x, nodes[curr_obj]->obj.y, nodes[curr_obj]->obj.z, transl);
		SetIdentityMatrix(nodes[curr_obj]->obj.model_matrix);
		MultiplyMatrix(transl, nodes[curr_obj]->obj.model_matrix, nodes[curr_obj]->obj.model_matrix);
	}
	
	//link parents to childs
	for(int i=0; i < num_objects; i++) {
		fscanf(fp,"%d\n", &child_idx);
		if(child_idx > 0 && child_idx < num_objects)
			nodes[i]->child_l = nodes[child_idx];

		fscanf(fp,"%d\n", &child_idx);
		if(child_idx > 0 && child_idx < num_objects)
			nodes[i]->child_r = nodes[child_idx];
	}

	fclose(fp);
	if(root_idx < num_objects)
		root = nodes[root_idx];
	free(nodes);
	return root;
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
