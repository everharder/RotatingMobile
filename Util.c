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

// r,g,b values are from 0 to 1
// h = [0,360], s = [0,1], v = [0,1]
//		if s == 0, then h = -1 (undefined)

/******************************************************************
* RGBtoHSV
*
* converts from RGB to HSV color space.
*******************************************************************/
void RGBtoHSV(GLfloat rgb[4], GLfloat hsv[4]){
	GLfloat r, g, b, min, max, delta;
	
	r = rgb[0];
	g = rgb[1];
	b = rgb[2];
	min = fmin(fmin(r, g), b);
	max = fmax(fmax(r, g), b);
	hsv[2] = max;				// v
	delta = max - min;

	if(max == 0){
		// r = g = b = 0		
		hsv[1] = 0;			// s = 0
		hsv[0] = -1;			// h is undefined
	}
	else{
		hsv[1] = delta / max;		// s

		if(r == max)
			hsv[0] = 60 * (g - b) / delta;	// between yellow & magenta
		else if( g == max )
			hsv[0] = 60 * (2 + (b - r) / delta);	// between cyan & yellow
		else
			hsv[0] = 60 * (4 + (r - g) / delta);	// between magenta & cyan
		
		if(hsv[0] < 0)	hsv[0] += 360;
	}

	hsv[3] = rgb[3];
}

/******************************************************************
* HSVtoRGB
*
* converts from HSV to RGB color space.
*******************************************************************/
void HSVtoRGB(GLfloat hsv[4], GLfloat rgb[4]){
	GLfloat h, s, v, f, p, q, t;
	int i;

	h = hsv[0];
	s = hsv[1];
	v = hsv[2];

	if(s == 0) {
		// achromatic (grey)
		rgb[0] = v;
		rgb[1] = v;
		rgb[2] = v;
	}
	else {
		h = h / 60;			// sector 0 to 5
		i = floor(h);
		f = h - i;			// factorial part of h
		p = v * (1 - s);
		q = v * (1 - s * f);
		t = v * (1 - s * (1 - f));

		switch(i) {
			case 0:
				rgb[0] = v;
				rgb[1] = t;
				rgb[2] = p;
				break;
			case 1:
				rgb[0] = q;
				rgb[1] = v;
				rgb[2] = p;
				break;
			case 2:
				rgb[0] = p;
				rgb[1] = v;
				rgb[2] = t;
				break;
			case 3:
				rgb[0] = p;
				rgb[1] = q;
				rgb[2] = v;
				break;
			case 4:
				rgb[0] = t;
				rgb[1] = p;
				rgb[2] = v;
				break;
			case 5:
				rgb[0] = v;
				rgb[1] = p;
				rgb[2] = q;
				break;
			default:
				break;
		}
	}

	rgb[3] = hsv[3];
}

/******************************************************************
* findNormals
*
* determines all normal vectors (one per triangle) of one object.
*******************************************************************/
/*
void findNormals(object_gl *object){
	object->normal_buffer_data = malloc(27 * object->num_vectr * sizeof(GLfloat));
				

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

		
//		object->normals[i*9] = 0.0;
//		object->normals[i*9+1] = 0.0;
//		object->normals[i*9+2] = 1.0;

//		object->normals[i*9+3] = 0.0;
//		object->normals[i*9+4] = 0.0;
//		object->normals[i*9+5] = 1.0;

//		object->normals[i*9+6] = 0.0;
//		object->normals[i*9+7] = 0.0;
//		object->normals[i*9+8] = 1.0;

		// Write normal vector to object buffer.
		
		free(vectr1);
		free(vectr2);
		free(vectr3);
		free(vectr12);
		free(vectr23);
		free(normal);
	}
}*/
