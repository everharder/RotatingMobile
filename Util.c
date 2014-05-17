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
* crossProduct
*
* determines the cross product of two 3D vectors.
*******************************************************************/
GLfloat* crossProduct(GLfloat *v1, GLfloat *v2){
	GLfloat *result = calloc(3, sizeof(GLfloat));

	result[0] = v1[1] * v2[2] - v1[2] * v2[1];
	result[1] = v1[2] * v2[0] - v1[0] * v2[2];
	result[2] = v1[0] * v2[1] - v1[1] * v2[0];

	return result;
}

/******************************************************************
* sumVectr
*
* determines the sum of two 3D vectors.
*******************************************************************/
GLfloat* sumVectr(GLfloat *v1, GLfloat *v2){
	GLfloat *result = calloc(3, sizeof(GLfloat));

	result[0] = v1[0] + v2[0];
	result[1] = v1[1] + v2[1];
	result[2] = v1[2] + v2[2];

	return result;
}

/******************************************************************
* sumVectr
*
* determines the difference of two 3D vectors.
*******************************************************************/
GLfloat* diffVectr(GLfloat *v1, GLfloat *v2){
	return sumVectr(v1, mulVectr(v2, -1));
}

/******************************************************************
* mulVectr
*
* multiplies one 3D vector with a scalar.
*******************************************************************/
GLfloat* mulVectr(GLfloat *v, float c){
	GLfloat *result = calloc(3, sizeof(GLfloat));

	result[0] = c * v[0];
	result[1] = c * v[1];
	result[2] = c * v[2];

	return result;
}
