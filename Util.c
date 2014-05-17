#include "Util.h"

/******************************************************************
* dotProduct
*
* determines the dot product of two 3D vectors.
*******************************************************************/
float dotProduct(GLfloat *v1, GLfloat *v2){
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]; 
}

/******************************************************************
* normVectr
*
* determines the norm (length) of two 3D vectors.
*******************************************************************/
float normVectr(GLfloat *v1, GLfloat *v2){
	return sqrt(v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]);
}

/******************************************************************
* crossProduct
*
* determines the cross product of two 3D vectors.
*******************************************************************/
void crossProduct(GLfloat *v1, GLfloat *v2, GLfloat *result){
	GLfloat temp[3] = {
		v1[1] * v2[2] - v1[2] * v2[1],
		v1[2] * v2[0] - v1[0] * v2[2],
		v1[0] * v2[1] - v1[1] * v2[0]
	};

	memcpy(result, temp, 3*sizeof(GLfloat));
}

/******************************************************************
* sumVectr
*
* determines the sum of two 3D vectors.
*******************************************************************/
void sumVectr(GLfloat *v1, GLfloat *v2, GLfloat *result){
	GLfloat temp[3] = {
		v1[0] + v2[0],
		v1[1] + v2[1],
		v1[2] + v2[2]
	};

	memcpy(result, temp, 3*sizeof(GLfloat));
}

/******************************************************************
* sumVectr
*
* determines the difference of two 3D vectors.
*******************************************************************/
void diffVectr(GLfloat *v1, GLfloat *v2, GLfloat *result){
	GLfloat temp[3] = {
			v1[0] - v2[0],
			v1[1] - v2[1],
			v1[2] - v2[2]
			};

	memcpy(result, temp, 3*sizeof(GLfloat));
}

/******************************************************************
* mulVectr
*
* multiplies one 3D vector with a scalar.
*******************************************************************/
void mulVectr(GLfloat *v, float c, GLfloat *result){
	GLfloat temp[3] = {
		c * v[0],
		c * v[1],
		c * v[2]
	};

	memcpy(result, temp, 3*sizeof(GLfloat));
}

/******************************************************************
* findNormals
*
* determines all normal vectors (one per triangle) of one object.
*******************************************************************/
void findNormals(object_gl *object){
	object->normals = malloc(27 * object->num_vectr * sizeof(GLfloat));
				

	// Go through all triangles of object.
	for (int i = 0; i < 3 * object->num_vectr; i+=3){
		if(object->vertx_per_vectr < 3) {
			object->normals[i] = 0.0;
			object->normals[i+1] = 0.0;
			object->normals[i+2] = 1.0;

			continue;
		}

		// Get indices of vertices which form one triangle.
		int index1 = object->index_buffer_data[i];
		int index2 = object->index_buffer_data[i+1];
		int index3 = object->index_buffer_data[i+2];

		GLfloat *vectr1 = malloc(object->vertx_per_vectr * sizeof(GLfloat));
		GLfloat *vectr2 = malloc(object->vertx_per_vectr * sizeof(GLfloat));
		GLfloat *vectr3 = malloc(object->vertx_per_vectr * sizeof(GLfloat));

		// Determine vector for each vertex in one triangle.
		for (int j = 0; j < object->vertx_per_vectr; j++){
			vectr1[j] = object->vertx_buffer_data[index1 * object->vertx_per_vectr + j];
			vectr2[j] = object->vertx_buffer_data[index2 * object->vertx_per_vectr + j];
			vectr3[j] = object->vertx_buffer_data[index3 * object->vertx_per_vectr + j];
		}

		// Determine normal vector of one triangle.
		GLfloat *vectr12 = malloc(object->vertx_per_vectr * sizeof(GLfloat));
		GLfloat *vectr23 = malloc(object->vertx_per_vectr * sizeof(GLfloat));
		GLfloat *vectr13 = malloc(object->vertx_per_vectr * sizeof(GLfloat));
		GLfloat *normal = malloc(object->vertx_per_vectr * sizeof(GLfloat));

		diffVectr(vectr2, vectr1, vectr12);
		diffVectr(vectr3, vectr2, vectr23);
		diffVectr(vectr3, vectr1, vectr13);
		
		crossProduct(vectr12, vectr13, normal);
		object->normals[i*9] = normal[0];
		object->normals[i*9+1] = normal[1];
		object->normals[i*9+2] = normal[2];

		crossProduct(vectr12, vectr23, normal);
		object->normals[i*9+3] = normal[0];
		object->normals[i*9+4] = normal[1];
		object->normals[i*9+5] = normal[2];

		crossProduct(vectr23, vectr13, normal);
		object->normals[i*9+6] = normal[0];
		object->normals[i*9+7] = normal[1];
		object->normals[i*9+8] = normal[2];	

		/*
		object->normals[i*9] = 0.0;
		object->normals[i*9+1] = 0.0;
		object->normals[i*9+2] = 1.0;

		object->normals[i*9+3] = 0.0;
		object->normals[i*9+4] = 0.0;
		object->normals[i*9+5] = 1.0;

		object->normals[i*9+6] = 0.0;
		object->normals[i*9+7] = 0.0;
		object->normals[i*9+8] = 1.0;*/

		// Write normal vector to object buffer.
		
		free(vectr1);
		free(vectr2);
		free(vectr3);
		free(vectr12);
		free(vectr23);
		free(normal);
	}
}
